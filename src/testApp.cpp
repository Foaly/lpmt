#include "testApp.h"
#include "stdio.h"
#include <iostream>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>


using namespace std;


int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL)
    {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        if (string(dirp->d_name) != "." && string(dirp->d_name) != "..")
        {
            files.push_back(string(dirp->d_name));
        }
    }
    closedir(dp);
    return 0;
}

testApp::testApp() : m_gui(this)
{

}

//--------------------------------------------------------------
void testApp::setup()
{
    ofDisableAntiAliasing();
    //ofSetVerticalSync(false);
    ofSetLogLevel(OF_LOG_WARNING);
    autoStart = false;

    // set window title
    ofSetWindowTitle("LPMT");

    // read xml config file
    ofxXmlSettings xmlConfigFile;
    const bool wasConfigLoadSuccessful = xmlConfigFile.loadFile("config.xml");
    if(!wasConfigLoadSuccessful) {
        std::cout << "WARNING: config file \"config.xml\" not found!" << std::endl << std::endl;
    }
    else {
        std::cout << "Loaded config file: \"config.xml\"" << std::endl;
    }

    #ifdef WITH_KINECT
    m_isKinectInitialized = kinect.setup();
    bCloseKinect = false;
    bOpenKinect = false;
    #endif

    // camera stuff
    m_cameras.clear();
    if(wasConfigLoadSuccessful)
    {
        xmlConfigFile.pushTag("cameras");
        // check how many cameras are defined in settings
        unsigned int numberOfCameras = 0;
        numberOfCameras = xmlConfigFile.getNumTags("camera");
        m_cameras.reserve(numberOfCameras); // reserve memory, so the iterators don't get invalidated by reallocation

        // cycle through defined cameras trying to initialize them and populate the cameras vector
        for (unsigned int i = 0; i < numberOfCameras; i++)
        {
            // read the requested parameters for the camera
            xmlConfigFile.pushTag("camera", i);
            const int requestedCameraWidth = xmlConfigFile.getValue("requestedWidth", 640);
            const int requestedCameraHeight = xmlConfigFile.getValue("requestedHeight", 480);
            const int cameraID = xmlConfigFile.getValue("id", 0);
            const int useForSnapshotBackground = xmlConfigFile.getValue("useForSnapshotBackround", 0);
            xmlConfigFile.popTag();

            // try initialize a video grabber
            ofVideoGrabber camera;
            camera.setDeviceID(cameraID);
            bool isVideoGrabberInitialized = false;
            isVideoGrabberInitialized = camera.initGrabber(requestedCameraWidth, requestedCameraHeight);
            const int actualCameraWidth = camera.getWidth();
            const int actualCameraHeight = camera.getHeight();

            // inform the user that the requested camera dimensions and the actual ones might differ
            std::cout << "Camera with ID " << cameraID << " asked for dimensions " << requestedCameraWidth << "x" << requestedCameraHeight;
            std::cout << " - actual size is " << actualCameraWidth << "x" << actualCameraHeight <<  std::endl;

            // check if the camera is available and eventually push it to cameras vector
            if (!isVideoGrabberInitialized || actualCameraWidth == 0 || actualCameraHeight == 0)
            {
                ofSystemAlertDialog("Camera with ID " + ofToString(cameraID) + " was requested, but was not found or is not available.");
                std::cout << "Camera with ID " << cameraID << " was requested, but was not found or is not available." << std::endl;
            }
            else
            {
                m_cameras.push_back(camera);
                // following vector is used for the combo box in SimpleGuiToo gui
                m_cameraIds.push_back(ofToString(cameraID));

                // if this camera is the first one or it is marked for being used
                // as the background snapshot camera, save a pointer to it
                if (useForSnapshotBackground == 1 || cameraID == 0)
                {
                    m_snapshotBackgroundCamera = m_cameras.end() - 1;
                    m_snapshotBackgroundTexture.allocate(camera.getWidth(), camera.getHeight(), GL_RGB);
                }
            }
        }
        xmlConfigFile.popTag();
    }

    // shared videos setup
    sharedVideos.clear();
    for(int i=0; i<8; i++)
    {
        ofVideoPlayer video;
        sharedVideos.push_back(video);
        sharedVideosFiles.push_back("");
    }

    //double click time
    m_doubleclickTime = 500; // 500 milliseconds
    m_timeLastClicked = 0;

    // rotation angle for surface-rotation visual feedback
    totRotationAngle = 0;

    if(ofGetScreenWidth()>1024 && ofGetScreenHeight()>800 )
    {
        WINDOW_W = 1024;
        WINDOW_H = 768;
    }
    else
    {
        WINDOW_W = 800;
        WINDOW_H = 600;
    }

    //we run at 60 fps!
    //ofSetVerticalSync(true);

    // initialize the splash screen
    m_isSplashScreenActive = true;
    m_SplashScreenImage.loadImage("lpmt_splash.png");

    //mask editing
    maskSetup = false;

    //grid editing
    gridSetup = false;

    // OSC setup
    int oscReceivePort = OSC_DEFAULT_PORT;
    if (wasConfigLoadSuccessful)
    {
        oscReceivePort = xmlConfigFile.getValue("OSC:LISTENING_PORT", OSC_DEFAULT_PORT);
    }
    std::cout << "Listening for OSC messages on port: " << oscReceivePort << std::endl;
    receiver.setup(oscReceivePort);

    current_msg_string = 0;
    oscControlMin = xmlConfigFile.getValue("OSC:GUI_CONTROL:SLIDER:MIN",0.0);
    oscControlMax = xmlConfigFile.getValue("OSC:GUI_CONTROL:SLIDER:MAX",1.0);
    cout << "osc control of gui sliders range: min=" << oscControlMin << " - max=" << oscControlMax << endl;


    // MIDI setup
    #ifdef WITH_MIDI
    // print input ports to console
	midiIn.listPorts();
	// open port by number
	//midiIn.openPort(1);
	//midiIn.openPort("IAC Pure Data In");	// by name
	midiIn.openVirtualPort("LPMT Input");	// open a virtual port

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midiIn.ignoreTypes(false, false, false);
	// add testApp as a listener
	midiIn.addListener(this);
	// print received messages to the console
	midiIn.setVerbose(true);
	//clear vectors used for midi-hotkeys coupling
	midiHotkeyMessages.clear();
	midiHotkeyKeys.clear();
    #endif

    oscHotkeyMessages.clear();
    oscHotkeyKeys.clear();

    bMidiHotkeyCoupling = false;
    bMidiHotkeyLearning = false;
    midiHotkeyPressed = -1;

    // we scan the video dir for videos
    //string videoDir = string("./data/video");
    //string videoDir =  ofToDataPath("video",true);
    //videoFiles = vector<string>();
    //getdir(videoDir,videoFiles);
    //string videos[videoFiles.size()];
    //for (unsigned int i = 0; i < videoFiles.size(); i++)
    //{
    //    videos[i]= videoFiles[i];
    //}

    // we scan the slideshow dir for videos
    //string slideshowDir = string("./data/slideshow");
    /*
    string slideshowDir = ofToDataPath("slideshow",true);
    slideshowFolders = vector<string>();
    getdir(slideshowDir,slideshowFolders);
    string slideshows[slideshowFolders.size()];
    for (unsigned int i = 0; i < slideshowFolders.size(); i++)
    {
        slideshows[i]= slideshowFolders[i];
    }
    */

    // initialize the load flags
    m_loadImageFlag = false;
    m_loadVideoFlag = false;
    m_loadSlideshowFlag = false;
    m_loadSharedVideo0Flag = false;
    m_loadSharedVideo1Flag = false;
    m_loadSharedVideo2Flag = false;
    m_loadSharedVideo3Flag = false;
    m_loadSharedVideo4Flag = false;
    m_loadSharedVideo5Flag = false;
    m_loadSharedVideo6Flag = false;
    m_loadSharedVideo7Flag = false;

    m_resetCurrentQuadFlag = false;


    #ifdef WITH_SYPHON
	// Syphon setup
	syphClient.setup();
    //syphClient.setApplicationName("Simple Server");
    syphClient.setServerName("");
    #endif

    // load shaders
    edgeBlendShader.load("shaders/blend.vert", "shaders/blend.frag");
    quadMaskShader.load("shaders/mask.vert", "shaders/mask.frag");
    chromaShader.load("shaders/chroma.vert", "shaders/chroma.frag");

    //ttf.loadFont("type/frabk.ttf", 11);
    ttf.loadFont("type/OpenSans-Regular.ttf", 11);
    // starts in quads setup mode
    isSetup = true;
    // starts running
    bStarted = true;
    // default is not using MostPixelsEver
    bMpe = false;
    // starts in windowed mode
    bFullscreen	= 0;
    // gui is on at start
    bGui = 1;
    ofSetWindowShape(WINDOW_W, WINDOW_H);

    // snap mode for surfaces corner is on
    bSnapOn = true;
    // number of surface to use as source in copy/paste (per default no quad is selected)
    m_sourceQuadForCopying = -1;

    //timeline defaults
    #ifdef WITH_TIMELINE
    useTimeline = false;
    timelineDurationSeconds = timelinePreviousDuration = 100.0;
    #endif

    // snapshot background texture is turned off by default
    m_isSnapshotTextureOn = false;


    // initializes layers array
    for(int i = 0; i < 36; i++)
    {
        layers[i] = -1;
    }

    // defines the first 4 default quads
    #ifdef WITH_KINECT
        #ifdef WITH_SYPHON
        quads[0].setup(ofPoint(0.0, 0.0), ofPoint(0.5, 0.0), ofPoint(0.5, 0.5), ofPoint(0.0, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
        #else
        quads[0].setup(ofPoint(0.0, 0.0), ofPoint(0.5, 0.0), ofPoint(0.5, 0.5), ofPoint(0.0, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
        #endif
    #else
        #ifdef WITH_SYPHON
        quads[0].setup(ofPoint(0.0, 0.0), ofPoint(0.5, 0.0), ofPoint(0.5, 0.5), ofPoint(0.0, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
        #else
        quads[0].setup(ofPoint(0.0, 0.0), ofPoint(0.5, 0.0), ofPoint(0.5, 0.5), ofPoint(0.0, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
        #endif
    #endif
    quads[0].quadNumber = 0;
    #ifdef WITH_KINECT
        #ifdef WITH_SYPHON
        quads[1].setup(ofPoint(0.5, 0.0), ofPoint(1.0, 0.0), ofPoint(1.0, 0.5), ofPoint(0.5, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
        #else
        quads[1].setup(ofPoint(0.5, 0.0), ofPoint(1.0, 0.0), ofPoint(1.0, 0.5), ofPoint(0.5, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
        #endif
    #else
        #ifdef WITH_SYPHON
        quads[1].setup(ofPoint(0.5, 0.0), ofPoint(1.0, 0.0), ofPoint(1.0, 0.5), ofPoint(0.5, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
        #else
        quads[1].setup(ofPoint(0.5, 0.0), ofPoint(1.0, 0.0), ofPoint(1.0, 0.5), ofPoint(0.5, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
        #endif
    #endif
    quads[1].quadNumber = 1;
    #ifdef WITH_KINECT
        #ifdef WITH_SYPHON
        quads[2].setup(ofPoint(0.0, 0.5), ofPoint(0.5, 0.5), ofPoint(0.5, 1.0), ofPoint(0.0, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
        #else
        quads[2].setup(ofPoint(0.0, 0.5), ofPoint(0.5, 0.5), ofPoint(0.5, 1.0), ofPoint(0.0, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
        #endif
    #else
        #ifdef WITH_SYPHON
        quads[2].setup(ofPoint(0.0, 0.5), ofPoint(0.5, 0.5), ofPoint(0.5, 1.0), ofPoint(0.0, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
        #else
        quads[2].setup(ofPoint(0.0, 0.5), ofPoint(0.5, 0.5), ofPoint(0.5, 1.0), ofPoint(0.0, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
        #endif
    #endif
    quads[2].quadNumber = 2;
    #ifdef WITH_KINECT
        #ifdef WITH_SYPHON
        quads[3].setup(ofPoint(0.5, 0.5), ofPoint(1.0, 0.5) ,ofPoint(1.0, 1.0), ofPoint(0.5, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
        #else
        quads[3].setup(ofPoint(0.5, 0.5), ofPoint(1.0, 0.5) ,ofPoint(1.0, 1.0), ofPoint(0.5, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
        #endif
    #else
        #ifdef WITH_SYPHON
        quads[3].setup(ofPoint(0.5, 0.5), ofPoint(1.0, 0.5) ,ofPoint(1.0, 1.0), ofPoint(0.5, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
        #else
        quads[3].setup(ofPoint(0.5, 0.5), ofPoint(1.0, 0.5) ,ofPoint(1.0, 1.0), ofPoint(0.5, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
        #endif
    #endif
    quads[3].quadNumber = 3;
    // define last one as active quad
    activeQuad = 3;
    quads[activeQuad].isActive = true;
    // number of total quads, to be modified later at each quad insertion
    nOfQuads = 4;
    layers[0] = 0;
    quads[0].layer = 0;
    layers[1] = 1;
    quads[1].layer = 1;
    layers[2] = 2;
    quads[2].layer = 2;
    layers[3] = 3;
    quads[3].layer = 3;

    // timeline stuff initialization
    #ifdef WITH_TIMELINE
    timelineSetup(timelineDurationSeconds);
    #endif

    // GUI STUFF ---------------------------------------------------
    m_gui.setupPages();
    m_gui.updatePages(quads[activeQuad]);
    m_gui.showPage(2);

    // timeline off at start
    bTimeline = false;
    #ifdef WITH_TIMELINE
    timeline.setCurrentPage(ofToString(activeQuad));
    timeline.hide();
    timeline.disable();
    // if timeline autostart is defined in timeline it starts timeline playing
    if(wasConfigLoadSuccessful)
    {
        float timelineConfigDuration = xmlConfigFile.getValue("TIMELINE:DURATION",10);
        timelineDurationSeconds = timelinePreviousDuration = timelineConfigDuration;
        timeline.setDurationInSeconds(timelineDurationSeconds);
        if(xmlConfigFile.getValue("TIMELINE:AUTOSTART",0))
        {
            timeline.togglePlay();
        }
    }
    #endif

    if(wasConfigLoadSuccessful)
    {
        autoStart = xmlConfigFile.getValue("PROJECTION:AUTO",0);
    }

    std::cout << "Setup done! playing now" << std::endl << std::endl;

    if(autoStart)
    {
        loadSettingsFromXMLFile("_lpmt_settings.xml");
        m_gui.updatePages(quads[activeQuad]);

        isSetup = false;
        m_gui.hide();
        bGui = false;
        for(int i = 0; i < 36; i++)
        {
            if (quads[i].initialized)
            {
                quads[i].isSetup = false;
            }
        }
        bFullscreen = true;
        ofSetFullscreen(true);
    }

}

void testApp::exit()
{

}

void testApp::mpeSetup()
{
    stopProjection();
    bMpe = true;
    // MPE stuff
    lastFrameTime = ofGetElapsedTimef();
    client.setup("mpe_client_settings.xml", true); //false means you can use backthread
    ofxMPERegisterEvents(this);
    //resync();
    startProjection();
    client.start();
    ofSetBackgroundAuto(false);
}


//--------------------------------------------------------------
void testApp::prepare()
{
    // check for waiting OSC messages
    while( receiver.hasWaitingMessages() )
    {
        parseOsc();
    }

    if (bStarted)
    {

        // updates shared video sources
        for(int i=0; i<8; i++)
        {
            if(sharedVideos[i].isLoaded())
            {
                sharedVideos[i].update();
            }
        }


        //check if load project button in the GUI was pressed
        if(m_loadProjectFlag)
        {
            m_loadProjectFlag = false;
            loadProject();
        }

        //check if save project button in the GUI was pressed
        if(m_saveProjectFlag)
        {
            m_saveProjectFlag = false;
            saveProject();
        }

        // check if image load button in the GUI was pressed
        if(m_loadImageFlag)
        {
            m_loadImageFlag = false;
            openImageFile();
        }

        // check if video load button in the GUI was pressed
        if(m_loadVideoFlag)
        {
            m_loadVideoFlag = false;
            openVideoFile();
        }

        // check if shared video load buttons in the GUI were pressed
        if(m_loadSharedVideo0Flag)
        {
            m_loadSharedVideo0Flag = false;
            openSharedVideoFile(0);
        }
        else if(m_loadSharedVideo1Flag)
        {
            m_loadSharedVideo1Flag = false;
            openSharedVideoFile(1);
        }
        else if(m_loadSharedVideo2Flag)
        {
            m_loadSharedVideo2Flag = false;
            openSharedVideoFile(2);
        }
        else if(m_loadSharedVideo3Flag)
        {
            m_loadSharedVideo3Flag = false;
            openSharedVideoFile(3);
        }
        else if(m_loadSharedVideo4Flag)
        {
            m_loadSharedVideo4Flag = false;
            openSharedVideoFile(4);
        }
        else if(m_loadSharedVideo5Flag)
        {
            m_loadSharedVideo5Flag = false;
            openSharedVideoFile(5);
        }
        else if(m_loadSharedVideo6Flag)
        {
            m_loadSharedVideo6Flag = false;
            openSharedVideoFile(6);
        }
        else if(m_loadSharedVideo7Flag)
        {
            m_loadSharedVideo7Flag = false;
            openSharedVideoFile(7);
        }
        // check if slide show load button in the GUI was pressed
       if(m_loadSlideshowFlag)
       {
            m_loadSlideshowFlag = false;
            loadSlideshow();
        }

        //check if quad dimensions reset button in the GUI was pressed
        if(m_resetCurrentQuadFlag)
        {
            m_resetCurrentQuadFlag = false;
            quadDimensionsReset(activeQuad);
            quadPlacementReset(activeQuad);
        }

        //check if quad bezier spherize button in the GUI was pressed
        if(m_bezierSpherizeQuadFlag)
        {
            m_bezierSpherizeQuadFlag = false;
            quadBezierSpherize(activeQuad);
        }

        //check if quad bezier spherize strong button in the GUI was pressed
        if(m_bezierSpherizeQuadStrongFlag)
        {
            m_bezierSpherizeQuadStrongFlag = false;
            quadBezierSpherizeStrong(activeQuad);
        }

        //check if quad bezier reset button in the GUI was pressed
        if(m_bezierResetQuadFlag)
        {
            m_bezierResetQuadFlag = false;
            quadBezierReset(activeQuad);
        }

        // check if kinect close button in the GUI was pressed
        #ifdef WITH_KINECT
        if(bCloseKinect)
        {
            bCloseKinect = false;
            kinect.kinect.setCameraTiltAngle(0);
            kinect.kinect.close();
        }

        // check if kinect close button in the GUI was pressed
        if(bOpenKinect)
        {
            bOpenKinect = false;
            kinect.kinect.open();
        }
        #endif

        for (int i=0; i < m_cameras.size(); i++)
        {
            if (m_cameras[i].getHeight() > 0)  // isLoaded check
            {
                m_cameras[i].update();
            }
        }


        // sets default window background, grey in setup mode and black in projection mode
        if (isSetup)
        {
            ofBackground(20, 20, 20);
        }
        else
        {
            ofBackground(0, 0, 0);
        }
        //ofSetWindowShape(800, 600);

        // kinect update
        #ifdef WITH_KINECT
        if(m_isKinectInitialized)
        {
            kinect.update();
        }
        #endif

        //timeline update
        #ifdef WITH_TIMELINE
        if(timelineDurationSeconds != timelinePreviousDuration)
        {
            timelinePreviousDuration = timelineDurationSeconds;
            timeline.setDurationInSeconds(timelineDurationSeconds);
        }
        if(useTimeline)
        {
            timelineUpdate();
        }
        #endif

        // loops through initialized quads and runs update, setting the border color as well
        for(int j = 0; j < 36; j++)
        {
            int i = layers[j];
            if (i >= 0)
            {
                if (quads[i].initialized)
                {
                    quads[i].update();
                    // frame delay correction for Mpe sync
                    if(bMpe)
                    {
                        if(quads[i].videoBg && quads[i].video.isLoaded())
                        {
                            int mpeFrame = client.getFrameCount();
                            int totFrames = quads[i].video.getTotalNumFrames();
                            int videoFrame = quads[i].video.getCurrentFrame();
                            //quads[i].video.setFrame(mpeFrame%totFrames);
                            if(abs((mpeFrame%totFrames) - videoFrame) > 2) // TODO: testing different values
                            {
                                //cout << mpeFrame%totFrames << endl;
                                quads[i].video.setFrame(mpeFrame%totFrames);
                            }
                        }
                    }
                }
            }
        }

    }
}


//--------------------------------------------------------------
void testApp::dostuff()
{
    if (bStarted)
    {

        // if snapshot is on draws it as window background
        if (isSetup && m_isSnapshotTextureOn)
        {
            ofEnableAlphaBlending();
            ofSetHexColor(0xFFFFFF);
            m_snapshotBackgroundTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
            ofDisableAlphaBlending();
        }

        // loops through initialized quads and calls their draw function
        for(int j = 0; j < 36; j++)
        {
            int i = layers[j];
            if (i >= 0)
            {
                if (quads[i].initialized)
                {
                    quads[i].draw();
                }
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::update()
{
    if (!bMpe)
    {
        if (m_isSplashScreenActive)
        {
            // turn the splash screen image of after 10 seconds
            if (ofGetElapsedTimef() > 10.f)
            {
                m_isSplashScreenActive = ! m_isSplashScreenActive;
            }
        }
        prepare();
    }
}

//--------------------------------------------------------------
void testApp::draw()
{
    if (!bMpe)
    {
        dostuff();
    }

    if (isSetup)
    {
        if (bStarted)
        {
            // if we are rotating surface, draws a feedback rotation sector
            ofEnableAlphaBlending();
            ofFill();
            ofSetColor(219, 104, 0, 255); // orange
            rotationSector.draw();
            ofNoFill();
            ofDisableAlphaBlending();

            // in setup mode writes the number of the active quad at the bottom of the window
            ofSetHexColor(0xFFFFFF); // white
            ttf.drawString("active surface: " + ofToString(activeQuad), 30, ofGetHeight() - 25);

            if(maskSetup) {
                ofSetHexColor(0xFF0000);
                ttf.drawString("Mask-editing mode ", 170, ofGetHeight()-25);
            }
            if(bMidiHotkeyCoupling) {
                if(bMidiHotkeyLearning)
                {
                ofSetColor(255,255,0);
                ttf.drawString("waiting for MIDI or OSC message ", 170, ofGetHeight()-25);
                }
                else{
                ofSetColor(255,0,0);
                ttf.drawString("MIDI or OSC hotkey coupling ", 170, ofGetHeight()-25);
                }
                ofRect(2,2,ofGetWidth()-4,ofGetHeight()-4);
            }
            // draws gui
            m_gui.draw();
        }
    }

    #ifdef WITH_TIMELINE
    if (bTimeline)
    {
        timeline.draw();
    }
    #endif

    // if the splash screen is active, draw the splash screen image
    if (m_isSplashScreenActive)
    {
        ofEnableAlphaBlending();
        ofSetHexColor(0xFFFFFF); // white
        m_SplashScreenImage.draw((ofGetWidth() / 2) - 230, (ofGetHeight() / 2) - 110);
        ofDisableAlphaBlending();
    }
}


//--------------------------------------------------------------
void testApp::mpeFrameEvent(ofxMPEEventArgs& event)
{
    if (bMpe)
    {
        if(client.getFrameCount()<=1)
        {
            resync();
        }
        prepare();
        dostuff();
    }
}

//--------------------------------------------------------------
void testApp::mpeMessageEvent(ofxMPEEventArgs& event)
{
    //received a message from the server
}


void testApp::mpeResetEvent(ofxMPEEventArgs& event)
{
    //triggered if the server goes down, another client goes offline, or a reset was manually triggered in the server code
}




//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

    if(!bMidiHotkeyCoupling){
    // moves active layer one position up
    if ( key == '+' && !bTimeline && !bGui)
    {
        int position;
        int target;

        for(int i = 0; i < 35; i++)
        {
            if (layers[i] == quads[activeQuad].quadNumber)
            {
                position = i;
                target = i+1;
            }

        }
        if (layers[target] != -1)
        {
            int target_content = layers[target];
            layers[target] = quads[activeQuad].quadNumber;
            layers[position] = target_content;
            quads[activeQuad].layer = target;
            quads[target_content].layer = position;
        }
    }


    // moves active layer one position down
    if ( key == '-' && !bTimeline && !bGui)
    {
        int position;
        int target;

        for(int i = 0; i < 36; i++)
        {
            if (layers[i] == quads[activeQuad].quadNumber)
            {
                position = i;
                target = i-1;
            }

        }
        if (target >= 0)
        {
            if (layers[target] != -1)
            {
                int target_content = layers[target];
                layers[target] = quads[activeQuad].quadNumber;
                layers[position] = target_content;
                quads[activeQuad].layer = target;
                quads[target_content].layer = position;
            }
        }
    }


    // saves quads settings to an .xml project file in data directory
    if ( (key == 's' || key == 'S') && !bTimeline)
    {
        saveProject();
    }

    // let the user choose an .xml project file with all the quads settings and loads it
    if ((key == 'l') && !bTimeline)
    {
        loadProject();
    }

    // if cameras are connected, take a snapshot of the specified camera and uses it as window background
    if (key == 'w' && !bTimeline)
    {
        if (m_cameras.size() > 0)
        {
            m_isSnapshotTextureOn = !m_isSnapshotTextureOn;
            if (m_isSnapshotTextureOn)
            {
                const int width = m_snapshotBackgroundCamera->getWidth();
                const int height = m_snapshotBackgroundCamera->getHeight();
                m_snapshotBackgroundCamera->update();
                m_snapshotBackgroundTexture.allocate(width, height, GL_RGB);
                m_snapshotBackgroundTexture.loadData(m_snapshotBackgroundCamera->getPixels(), width, height, GL_RGB);
            }
        }
        else
        {
            std::cout << "Can't take a snapshot background picture. No camera connected." << std::endl;
        }
    }

    // loads an image file and uses it as window background
    if (key == 'W' && !bTimeline)
    {
        m_isSnapshotTextureOn = !m_isSnapshotTextureOn;
        if (m_isSnapshotTextureOn)
        {
            ofImage image(loadImageFromFile());
            m_snapshotBackgroundTexture.allocate(image.width, image.height, GL_RGB);
            m_snapshotBackgroundTexture.loadData(image.getPixels(), image.width, image.height, GL_RGB);
        }
    }

    // fills window with active quad
    if ( (key =='q' || key == 'Q') && !bTimeline)
    {
        if (isSetup)
        {
            quads[activeQuad].corners[0] = ofPoint(0.0, 0.0);
            quads[activeQuad].corners[1] = ofPoint(1.0, 0.0);
            quads[activeQuad].corners[2] = ofPoint(1.0, 1.0);
            quads[activeQuad].corners[3] = ofPoint(0.0, 1.0);
        }
    }

    // activates next quad
    if ( key =='>' && !bTimeline)
    {
        if (isSetup)
        {
            quads[activeQuad].isActive = false;
            activeQuad += 1;
            if (activeQuad > nOfQuads-1)
            {
                activeQuad = 0;
            }
            quads[activeQuad].isActive = true;
        }
        m_gui.updatePages(quads[activeQuad]);
    }

    // activates prev quad
    if ( key =='<' && !bTimeline)
    {
        if (isSetup)
        {
            quads[activeQuad].isActive = false;
            activeQuad -= 1;
            if (activeQuad < 0)
            {
                activeQuad = nOfQuads-1;
            }
            quads[activeQuad].isActive = true;
        }
        m_gui.updatePages(quads[activeQuad]);
    }

    // goes to first page of gui for active quad or, in mask edit mode, delete last drawn point
    if ( (key == 'z' || key == 'Z') && !bTimeline)
    {
        if(maskSetup && quads[activeQuad].m_maskPoints.size() > 0)
        {
            quads[activeQuad].m_maskPoints.pop_back();
        }
        else
        {
            m_gui.showPage(2);
        }
    }

    if ( key == OF_KEY_F1)
    {
        m_gui.showPage(2);
    }


    if ( (key == 'd' || key == 'D') && !bTimeline)
    {
        if(maskSetup && quads[activeQuad].m_maskPoints.size() > 0)
        {
            if (quads[activeQuad].bHighlightMaskPoint)
            {
                quads[activeQuad].m_maskPoints.erase(quads[activeQuad].m_maskPoints.begin() + quads[activeQuad].highlightedMaskPoint);
            }

        }
    }


    // goes to second page of gui for active quad
    if ( (key == 'x' || key == 'X' || key == OF_KEY_F2) && !bTimeline)
    {
        m_gui.showPage(3);
    }

    // goes to third page of gui for active quad or, in edit mask mode, clears mask
    if ( (key == 'c' || key == 'C') && !bTimeline)
    {
        if(maskSetup)
        {
            quads[activeQuad].m_maskPoints.clear();
        }
        else
        {
            m_gui.showPage(4);
        }
    }

    if (key == OF_KEY_F3)
    {
        m_gui.showPage(4);
    }

    // make currently active quad the source quad for copying
    // (3 corresponds to CTRL + C)
    if ( (key == 3) && !bTimeline)
    {
        m_sourceQuadForCopying = activeQuad;
    }

    // paste settings from source surface to currently active surface
    // (22 corresponds to CTRL + V)
    if ( (key == 22) && !bTimeline)
    {
        copyQuadSettings(m_sourceQuadForCopying);
    }

    // adds a new quad in the middle of the screen
    if ( key =='a' && !bTimeline)
    {
        if (isSetup)
        {
            if (nOfQuads < 36)
            {
                #ifdef WITH_KINECT
                    #ifdef WITH_SYPHON
                    quads[nOfQuads].setup(ofPoint(0.25, 0.25), ofPoint(0.75, 0.25), ofPoint(0.75, 0.75), ofPoint(0.25, 0.75), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
                    #else
                    quads[nOfQuads].setup(ofPoint(0.25, 0.25), ofPoint(0.75, 0.25), ofPoint(0.75, 0.75), ofPoint(0.25, 0.75), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
                    #endif
                #else
                    #ifdef WITH_SYPHON
                    quads[nOfQuads].setup(ofPoint(0.25, 0.25), ofPoint(0.75, 0.25), ofPoint(0.75, 0.75), ofPoint(0.25, 0.75), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
                    #else
                    quads[nOfQuads].setup(ofPoint(0.25, 0.25), ofPoint(0.75, 0.25), ofPoint(0.75, 0.75), ofPoint(0.25, 0.75), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
                    #endif
                #endif
                quads[nOfQuads].quadNumber = nOfQuads;
                layers[nOfQuads] = nOfQuads;
                quads[nOfQuads].layer = nOfQuads;
                quads[activeQuad].isActive = false;
                quads[nOfQuads].isActive = true;
                activeQuad = nOfQuads;
                ++nOfQuads;
                m_gui.updatePages(quads[activeQuad]);
                // add timeline page for new quad
                #ifdef WITH_TIMELINE
                timelineAddQuadPage(activeQuad);
                #endif
                // next line fixes a bug i've been tracking down for a looong time
                glDisable(GL_DEPTH_TEST);

            }
        }
    }

    // toggles setup mode
    if ( key ==' ' && !bTimeline)
    {
        if (isSetup)
        {
            isSetup = false;
            m_gui.hide();
            bGui = false;
            for(int i = 0; i < 36; i++)
            {
                if (quads[i].initialized)
                {
                    quads[i].isSetup = false;
                }
            }
        }
        else
        {
            isSetup = true;
            m_gui.show();
            bGui = true;
            for(int i = 0; i < 36; i++)
            {
                if (quads[i].initialized)
                {
                    quads[i].isSetup = true;
                }
            }
        }
    }

    // toggles fullscreen mode
    if(key == 'f' && !bTimeline)
    {

        bFullscreen = !bFullscreen;

        if(!bFullscreen)
        {
            ofSetWindowShape(WINDOW_W, WINDOW_H);
            ofSetFullscreen(false);
            // figure out how to put the window in the center:
            int screenW = ofGetScreenWidth();
            int screenH = ofGetScreenHeight();
            ofSetWindowPosition(screenW/2-WINDOW_W/2, screenH/2-WINDOW_H/2);
        }
        else if(bFullscreen == 1)
        {
            ofSetFullscreen(true);
        }
    }

    // toggles gui
    if(key == 'g' && !bTimeline)
    {
        if (maskSetup) {
            maskSetup = false;
            for(int i = 0; i < 36; i++)
                {
                    if (quads[i].initialized)
                    {
                        quads[i].isMaskSetup = false;
                    }
                }
        }
        m_gui.toggleDraw();
        bGui = !bGui;
    }

    // toggles mask editing
    if(key == 'm' && !bTimeline)
    {
        if (!bGui){
        maskSetup = !maskSetup;
        for(int i = 0; i < 36; i++)
            {
                if (quads[i].initialized)
                {
                    quads[i].isMaskSetup = !quads[i].isMaskSetup;
                }
            }
        }
    }

    // toggles bezier deformation editing
    if(key == 'b' && !bTimeline)
    {
        if (!bGui){
        gridSetup = !gridSetup;
        for(int i = 0; i < 36; i++)
            {
                if (quads[i].initialized)
                {
                    quads[i].isBezierSetup = !quads[i].isBezierSetup;
                }
            }
        }
    }

    if(key == '[' && !bTimeline)
    {
        m_gui.prevPage();
    }

    if(key == ']' && !bTimeline)
    {
        m_gui.nextPage();
    }

    // show general settings page of gui
    if(key == 'v' && !bTimeline)
    {
        m_gui.showPage(1);
    }

    // resyncs videos to start point in every quad
    if((key == 'r' || key == 'R') && !bTimeline)
    {
        resync();
    }


    // starts and stops rendering

    if(key == 'p' && !bTimeline)
    {
        startProjection();
    }

    if(key == 'o' && !bTimeline)
    {
        stopProjection();
    }

    if(key == 'n' && !bTimeline)
    {
        mpeSetup();
    }

    // displays help in system dialog
    if((key == 'h' || key == OF_KEY_F1) && !bTimeline)
    {
        ofBuffer buf = ofBufferFromFile("help_keys.txt");
        ofSystemAlertDialog(buf);
    }

    // show-hide stage when timeline is shown
    if(key == OF_KEY_F11 && bTimeline)
    {
        if(bStarted)
        {
            bStarted = false;
            for(int i = 0; i < 36; i++)
            {
                if (quads[i].initialized)
                {
                quads[i].isOn = false;
                    if (quads[i].videoBg && quads[i].video.isLoaded())
                    {
                        quads[i].video.setVolume(0);
                        quads[i].video.stop();
                    }
                }
            }
        }
        else if(!bStarted)
        {
            bStarted = true;
            for(int i = 0; i < 36; i++)
            {
                if (quads[i].initialized)
                {
                    quads[i].isOn = true;
                    if (quads[i].videoBg && quads[i].video.isLoaded())
                    {
                        quads[i].video.setVolume(quads[i].videoVolume);
                        quads[i].video.play();
                    }
                }
            }
        }
    }


    // toggle timeline
    #ifdef WITH_TIMELINE
    if(key == OF_KEY_F10)
    {
        bTimeline = !bTimeline;
        timeline.toggleShow();
        if(bTimeline)
        {
            timeline.enable();
            m_gui.hide();
            bGui = false;
        }
        else
        {
            //timeline.disable();
        }
    }

    // toggle timeline playing
    if(key == OF_KEY_F12)
    {
        timeline.togglePlay();
    }

    // toggle timeline BPM grid drawing
    if(key == OF_KEY_F9 && bTimeline)
    {
        timeline.toggleShowBPMGrid();
    }
    #endif

    if(key == '*' && !bTimeline)
    {
        if(m_cameras[quads[activeQuad].camNumber].getPixelFormat() == OF_PIXELS_RGBA)
        {
            m_cameras[quads[activeQuad].camNumber].setPixelFormat(OF_PIXELS_BGRA);
        }
        else if(m_cameras[quads[activeQuad].camNumber].getPixelFormat() == OF_PIXELS_BGRA)
        {
            m_cameras[quads[activeQuad].camNumber].setPixelFormat(OF_PIXELS_RGBA);
        }

    }

    // rotation of surface around its center
    if(key == '£' && !bTimeline)
    {
        ofMatrix4x4 rotation;
        ofMatrix4x4 centerToOrigin;
        ofMatrix4x4 originToCenter;
        ofMatrix4x4 resultingMatrix;
        centerToOrigin.makeTranslationMatrix(-quads[activeQuad].center);
        originToCenter.makeTranslationMatrix(quads[activeQuad].center);
        rotation.makeRotationMatrix(-5.0,0,0,1);
        resultingMatrix = centerToOrigin * rotation * originToCenter;
        for(int i=0; i<4; i++)
        {
            quads[activeQuad].corners[i] = quads[activeQuad].corners[i] * resultingMatrix;
        }
    }

    if(key == '$' && !bTimeline)
    {
        ofMatrix4x4 rotation;
        ofMatrix4x4 centerToOrigin;
        ofMatrix4x4 originToCenter;
        ofMatrix4x4 resultingMatrix;
        centerToOrigin.makeTranslationMatrix(-quads[activeQuad].center);
        originToCenter.makeTranslationMatrix(quads[activeQuad].center);
        rotation.makeRotationMatrix(5.0,0,0,1);
        resultingMatrix = centerToOrigin * rotation * originToCenter;
        for(int i=0; i<4; i++)
        {
            quads[activeQuad].corners[i] = quads[activeQuad].corners[i] * resultingMatrix;
        }
    }
    }

    else
        {
            bMidiHotkeyLearning = true;
            midiHotkeyPressed = key;
        }

    if ( key == OF_KEY_F4)
    {
        bMidiHotkeyCoupling = !bMidiHotkeyCoupling;
        bMidiHotkeyLearning = false;
        midiHotkeyPressed = -1;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
}


//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{
    const ofPoint mousePosition(x, y);

    if (isSetup && !bGui && !maskSetup && !gridSetup && !bTimeline)
    {
        float smallestDist = 1.0;
        m_selectedCorner = -1;

        for(int i = 0; i < 4; i++)
        {
            float distx = quads[activeQuad].corners[i].x - (float)x/ofGetWidth();
            float disty = quads[activeQuad].corners[i].y - (float)y/ofGetHeight();
            float dist  = sqrt( distx * distx + disty * disty);

            if(dist < smallestDist && dist < 0.05) // value for dist threshold can vary between 0.1-0.05, fine tune it
            {
                m_selectedCorner = i;
                smallestDist = dist;
            }
        }

        if(m_selectedCorner >= 0)
            {
                quads[activeQuad].bHighlightCorner = true;
                quads[activeQuad].highlightedCorner = m_selectedCorner;
            }
        else
            {
                quads[activeQuad].bHighlightCorner = false;
                quads[activeQuad].highlightedCorner = -1;

                // distance from center
                float distx = quads[activeQuad].center.x - (float)x / ofGetWidth();
                float disty = quads[activeQuad].center.y - (float)y/ofGetHeight();
                float dist  = sqrt( distx * distx + disty * disty);
                if(dist < 0.05)
                {
                    quads[activeQuad].bHighlightCenter = true;
                }
                else {quads[activeQuad].bHighlightCenter = false;}

                // distance from rotation grab point
                ofPoint rotationGrabPoint;
                //rotationGrabPoint.x = (quads[activeQuad].corners[2].x - quads[activeQuad].corners[1].x)/2 + quads[activeQuad].corners[1].x;
                //rotationGrabPoint.y = (quads[activeQuad].corners[2].y - quads[activeQuad].corners[1].y)/2 + quads[activeQuad].corners[1].y;
                //rotationGrabPoint = ((quads[activeQuad].corners[2]+quads[activeQuad].corners[1])/2+quads[activeQuad].center)/2;
                rotationGrabPoint = (quads[activeQuad].center);
                rotationGrabPoint.x = rotationGrabPoint.x + 0.1;
                float rotationDistx = rotationGrabPoint.x - (float)x / ofGetWidth();
                float rotationDisty = rotationGrabPoint.y - (float)y/ofGetHeight();
                float rotationDist = sqrt(rotationDistx*rotationDistx + rotationDisty*rotationDisty);
                if(rotationDist < 0.05)
                {
                    quads[activeQuad].bHighlightRotation = true;
                }
                else {quads[activeQuad].bHighlightRotation = false;}
            }
    }

    else if (maskSetup && !gridSetup && !bTimeline)
    {
        float smallestDist = sqrt( ofGetWidth() * ofGetWidth() + ofGetHeight() * ofGetHeight());;
        int whichPoint = -1;
        ofVec3f warped;
        for(int i = 0; i < quads[activeQuad].m_maskPoints.size(); i++)
        {
            warped = quads[activeQuad].getWarpedPoint(mousePosition);
            float distx = (float)quads[activeQuad].m_maskPoints[i].x * ofGetWidth() - (float)warped.x;
            float disty = (float)quads[activeQuad].m_maskPoints[i].y * ofGetHeight()- (float)warped.y;
            float dist  = sqrt( distx * distx + disty * disty);

            if(dist < smallestDist && dist < 20.0)
            {
                whichPoint = i;
                smallestDist = dist;
            }
        }
        if(whichPoint >= 0)
            {
                quads[activeQuad].bHighlightMaskPoint = true;
                quads[activeQuad].highlightedMaskPoint = whichPoint;
            }
        else
            {
                quads[activeQuad].bHighlightMaskPoint = false;
                quads[activeQuad].highlightedMaskPoint = -1;
            }
    }

    else if (gridSetup && !maskSetup && !bTimeline)
    {
        float smallestDist = sqrt( ofGetWidth() * ofGetWidth() + ofGetHeight() * ofGetHeight());;
        int whichPointRow = -1;
        int whichPointCol = -1;
        ofVec3f warped;

        if(quads[activeQuad].bBezier)
        {
        for(int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                warped = quads[activeQuad].getWarpedPoint(mousePosition);
                float distx = (float)quads[activeQuad].bezierPoints[i][j][0] * ofGetWidth() - (float)warped.x;
                float disty = (float)quads[activeQuad].bezierPoints[i][j][1] * ofGetHeight() - (float)warped.y;
                float dist  = sqrt( distx * distx + disty * disty);

                if(dist < smallestDist && dist < 20.0)
                {
                    whichPointRow = i;
                    whichPointCol = j;
                    smallestDist = dist;
                }
            }
        }
        }

        else if(quads[activeQuad].bGrid)
        {
        for(int i = 0; i <= quads[activeQuad].gridRows; i++)
        {
            for (int j = 0; j <= quads[activeQuad].gridColumns; j++)
            {
                warped = quads[activeQuad].getWarpedPoint(mousePosition);
                float distx = (float)quads[activeQuad].gridPoints[i][j][0] * ofGetWidth() - (float)warped.x;
                float disty = (float)quads[activeQuad].gridPoints[i][j][1] * ofGetHeight() - (float)warped.y;
                float dist  = sqrt( distx * distx + disty * disty);

                if(dist < smallestDist && dist < 20.0)
                {
                    whichPointRow = i;
                    whichPointCol = j;
                    smallestDist = dist;
                }
            }
        }
        }

        if(whichPointRow >= 0)
            {
                quads[activeQuad].bHighlightCtrlPoint = true;
                quads[activeQuad].highlightedCtrlPointRow = whichPointRow;
                quads[activeQuad].highlightedCtrlPointCol = whichPointCol;
            }
        else
            {
                quads[activeQuad].bHighlightCtrlPoint = false;
                quads[activeQuad].highlightedCtrlPointRow = -1;
                quads[activeQuad].highlightedCtrlPointCol = -1;
            }
    }
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    const ofPoint mousePosition(x, y);

    // quad movement code
    if (isSetup && !bGui && !maskSetup && !gridSetup && !bTimeline)
    {
        // check if one of the corners is selected
        if(m_selectedCorner >= 0)
        {
            float normalizedX = (float)x / ofGetWidth();
            float normalizedY = (float)y / ofGetHeight();

            // move the selected corner
            quads[activeQuad].corners[m_selectedCorner] = ofPoint(normalizedX, normalizedY);
        }
        else
        {
            // if no corner is selected, check if we can move or rotate whole quad
            //by dragging its center and rotation mark
            if(quads[activeQuad].bHighlightCenter) // TODO: verifiy if threshold value is good for distance
            {
                ofPoint movement(mousePosition - startDrag);

                // move the entire quad
                for(int i = 0; i < 4; i++)
                {
                    quads[activeQuad].corners[i].x += ((float)movement.x / ofGetWidth());
                    quads[activeQuad].corners[i].y += ((float)movement.y / ofGetHeight());
                }
                startDrag = mousePosition;
            }
            // rotate the quad
            else if(quads[activeQuad].bHighlightRotation)
            {
                float angle;
                // quad center in pixel coordinates
                ofPoint center(quads[activeQuad].center.x * ofGetWidth(), quads[activeQuad].center.y * ofGetHeight());
                ofPoint vec1 = (startDrag - center);
                ofPoint vec2 = (mousePosition - center);
                angle = ofRadToDeg(std::atan2(vec2.y, vec2.x) - std::atan2(vec1.y, vec1.x));

                totRotationAngle += angle;
                rotationSector.clear();
                rotationSector.addVertex(center);
                rotationSector.lineTo(center.x+(0.025*ofGetWidth()),center.y);
                rotationSector.arc(center, 0.025*ofGetWidth(), 0.025*ofGetWidth(), 0, totRotationAngle, 40);
                rotationSector.close();

                ofMatrix4x4 rotation;
                ofMatrix4x4 centerToOrigin;
                ofMatrix4x4 originToCenter;
                ofMatrix4x4 resultingMatrix;
                centerToOrigin.makeTranslationMatrix(-quads[activeQuad].center);
                originToCenter.makeTranslationMatrix(quads[activeQuad].center);
                rotation.makeRotationMatrix(angle, 0, 0, 1);
                resultingMatrix = centerToOrigin * rotation * originToCenter;
                for(int i=0; i<4; i++)
                {
                    quads[activeQuad].corners[i] = quads[activeQuad].corners[i] * resultingMatrix;
                }
                startDrag = mousePosition;
            }
        }
    }
    else if(maskSetup && quads[activeQuad].bHighlightMaskPoint && !bTimeline)
    {
        // in mask setup mode, move the selected mask point
        const ofPoint warpedPoint = quads[activeQuad].getWarpedPoint(mousePosition);
        const ofPoint normalizedPoint(warpedPoint.x / ofGetWidth(), warpedPoint.y / ofGetHeight());

        quads[activeQuad].m_maskPoints[quads[activeQuad].highlightedMaskPoint] = normalizedPoint;
    }

    else if(gridSetup && quads[activeQuad].bHighlightCtrlPoint && !bTimeline)
    {
        const int currentRow = quads[activeQuad].highlightedCtrlPointRow;
        const int currentCol = quads[activeQuad].highlightedCtrlPointCol;
        const ofPoint warpedPoint = quads[activeQuad].getWarpedPoint(mousePosition);

        if(quads[activeQuad].bBezier)
        {
            quads[activeQuad].bezierPoints[currentRow][currentCol][0] = (float)warpedPoint.x / ofGetWidth();
            quads[activeQuad].bezierPoints[currentRow][currentCol][1] = (float)warpedPoint.y / ofGetHeight();
        }
        else if(quads[activeQuad].bGrid)
        {
            quads[activeQuad].gridPoints[currentRow][currentCol][0] = (float)warpedPoint.x / ofGetWidth();
            quads[activeQuad].gridPoints[currentRow][currentCol][1] = (float)warpedPoint.y / ofGetHeight();
        }
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    const ofPoint mousePosition(x, y);

    rotationSector.clear();
    // this is used for dragging the whole quad using its centroid
    startDrag = mousePosition;

    // deactivate the splash screen on click
    if (m_isSplashScreenActive)
    {
        m_isSplashScreenActive = !m_isSplashScreenActive;
    }

    if (isSetup && !bGui && !bTimeline)
    {

        if(maskSetup && !gridSetup)
        {
            // if we are in mask setup mode and no mask point is selected, add a new mask point
            if (!quads[activeQuad].bHighlightMaskPoint)
            {
                quads[activeQuad].maskAddPoint(mousePosition);
            }
        }

        else
        {
            // check if the user double-clicked on a different quad and make it the active one
            unsigned long now = ofGetElapsedTimeMillis();
            if (m_timeLastClicked != 0 && now - m_timeLastClicked < m_doubleclickTime)
            {
                activateClosestQuad(mousePosition);
            }
            m_timeLastClicked = now;

            // check if the user clicked on one of active quad's corners and select it
            float smallestDist = 1.0;
            m_selectedCorner = -1;

            for(int i = 0; i < 4; i++)
            {
                float distx = quads[activeQuad].corners[i].x - (float)x/ofGetWidth();
                float disty = quads[activeQuad].corners[i].y - (float)y/ofGetHeight();
                float dist  = sqrt( distx * distx + disty * disty);

                if(dist < smallestDist && dist < 0.05)
                {
                    m_selectedCorner = i;
                    smallestDist = dist;
                }
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased()
{
    totRotationAngle = 0;
    rotationSector.clear();
    if (isSetup && !bGui && !bTimeline)
    {

    if (m_selectedCorner >= 0)
    {
        // snap detection for near quads
        float smallestDist = 1.0;
        int snapQuad = -1;
        int snapCorner = -1;
        for (int i = 0; i < 36; i++)
        {
            if ( i != activeQuad && quads[i].initialized)
            {
                for(int j = 0; j < 4; j++)
                {
                    float distx = quads[activeQuad].corners[m_selectedCorner].x - quads[i].corners[j].x;
                    float disty = quads[activeQuad].corners[m_selectedCorner].y - quads[i].corners[j].y;
                    float dist = sqrt( distx * distx + disty * disty);
                    // to tune snapping change dist value inside next if statement
                    if (dist < smallestDist && dist < 0.0075)
                    {
                        snapQuad = i;
                        snapCorner = j;
                        smallestDist = dist;
                    }
                }
            }
        }
        if (snapQuad >= 0 && snapCorner >= 0 && bSnapOn)
        {
            quads[activeQuad].corners[m_selectedCorner].x = quads[snapQuad].corners[snapCorner].x;
            quads[activeQuad].corners[m_selectedCorner].y = quads[snapQuad].corners[snapCorner].y;
        }
    }
    m_selectedCorner = -1;
    quads[activeQuad].bHighlightCorner = false;
    }
}


void testApp::windowResized(int w, int h)
{
            #ifdef WITH_TIMELINE
            timeline.setWidth(w);
            #endif
            for(int i = 0; i < 36; i++)
            {
                if (quads[i].initialized)
                {
                    quads[i].bHighlightCorner = false;
                    quads[i].allocateFbo(ofGetWidth(),ofGetHeight());
                    quadDimensionsReset(i);
                }
            }
}



//---------------------------------------------------------------
void testApp::quadDimensionsReset(int q)
{
    quads[q].quadW = ofGetWidth();
    quads[q].quadH = ofGetHeight();
}

//---------------------------------------------------------------
void testApp::quadPlacementReset(int q)
{
    quads[q].quadDispX = 0;
    quads[q].quadDispY = 0;
}

//---------------------------------------------------------------
void testApp::quadBezierSpherize(int q)
{
    float w = (float)ofGetWidth();
    float h = (float)ofGetHeight();
    float k = (sqrt(2)-1)*4/3;
    quads[q].bBezier = true;

    float tmp_bezierPoints[4][4][3] =
    {
        {   {0*h/w+(0.5*(w/h-1))*h/w, 0, 0},{0.5*k*h/w+(0.5*(w/h-1))*h/w, -0.5*k, 0},    {(1.0*h/w)-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, -0.5*k, 0},    {1.0*h/w+(0.5*(w/h-1))*h/w, 0, 0}    },
        {   {0*h/w-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 0.5*k, 0},        {0*h/w+(0.5*(w/h-1))*h/w, 0, 0},  {1.0*h/w+(0.5*(w/h-1))*h/w, 0, 0},  {1.0*h/w+(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 0.5*k, 0}  },
        {   {0*h/w-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0-0.5*k, 0},        {0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0},  {1.0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0},  {1.0*h/w+(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0-0.5*k, 0}  },
        {   {0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0}, {0.5*k*h/w+(0.5*(w/h-1))*h/w, 1.0+0.5*k, 0},  {(1.0*h/w)-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0+0.5*k, 0},  {1.0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0}  }
    };

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 3; ++k) {
				 quads[q].bezierPoints [i][j][k] = tmp_bezierPoints[i][j][k];
			}
		}
	}
	/*  quads[q].bezierPoints =
    {
        {   {(0.5*w/h-0.5)*h/w, 0, 0},  {0.5*(k+w/h-1)*h/w, -0.5*k, 0},    {0.5*(1-k+w/h)*h/w, -0.5*k, 0},    {1.0*h/w+(0.5*(w/h-1))*h/w, 0, 0}    },
        {   {0*h/w-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 0.5*k, 0},        {0*h/w+(0.5*(w/h-1))*h/w, 0, 0},  {1.0*h/w+(0.5*(w/h-1))*h/w, 0, 0},  {1.0*h/w+(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 0.5*k, 0}  },
        {   {0*h/w-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0-0.5*k, 0},        {0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0},  {1.0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0},  {1.0*h/w+(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0-0.5*k, 0}  },
        {   {0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0},        {0.5*k*h/w+(0.5*(w/h-1))*h/w, 1.0+0.5*k, 0},  {(1.0*h/w)-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0+0.5*k, 0},  {1.0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0}  }
    }; */
}

//---------------------------------------------------------------
void testApp::quadBezierSpherizeStrong(int q)
{
    float w = (float)ofGetWidth();
    float h = (float)ofGetHeight();
    float k = (sqrt(2)-1)*4/3;
    quads[q].bBezier = true;

    float tmp_bezierPoints[4][4][3] =
    {
        {   {0*h/w+(0.5*(w/h-1))*h/w, 0, 0},  {0.5*k*h/w+(0.5*(w/h-1))*h/w, -0.5*k, 0},    {(1.0*h/w)-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, -0.5*k, 0},    {1.0*h/w+(0.5*(w/h-1))*h/w, 0, 0}    },
        {   {0*h/w-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 0.5*k, 0},        {0*h/w-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, -0.5*k, 0},  {1.0*h/w+(0.5*k*h/w)+(0.5*(w/h-1))*h/w, -0.5*k, 0},  {1.0*h/w+(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 0.5*k, 0}  },
        {   {0*h/w-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0-0.5*k, 0},        {0*h/w-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0+0.5*k, 0},  {1.0*h/w+(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0+0.5*k, 0},  {1.0*h/w+(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0-0.5*k, 0}  },
        {   {0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0},        {0.5*k*h/w+(0.5*(w/h-1))*h/w, 1.0+0.5*k, 0},  {(1.0*h/w)-(0.5*k*h/w)+(0.5*(w/h-1))*h/w, 1.0+0.5*k, 0},  {1.0*h/w+(0.5*(w/h-1))*h/w, 1.0, 0}  }
    };

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 3; ++k) {
				quads[q].bezierPoints [i][j][k] = tmp_bezierPoints[i][j][k];
			}
		}
	}
}

//---------------------------------------------------------------
void testApp::quadBezierReset(int q)
{
    quads[q].bBezier = true;
    float tmp_bezierPoints[4][4][3] =
    {
        {   {0, 0, 0},          {0.333, 0, 0},    {0.667, 0, 0},    {1.0, 0, 0}    },
        {   {0, 0.333, 0},        {0.333, 0.333, 0},  {0.667, 0.333, 0},  {1.0, 0.333, 0}  },
        {   {0, 0.667, 0},        {0.333, 0.667, 0},  {0.667, 0.667, 0},  {1.0, 0.667, 0}  },
        {   {0, 1.0, 0},        {0.333, 1.0, 0},  {0.667, 1.0, 0},  {1.0, 1.0, 0}  }
    };

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			for (int k = 0; k < 3; ++k) {
				quads[q].bezierPoints [i][j][k] = tmp_bezierPoints[i][j][k];
			}
		}
	}
}


//---------------------------------------------------------------
// This method activates the quad, whos center is closest to the given point.
// Since we don't have a proper quad/point intersection test, the point must also be
// within a radius the size of a 10th of the windows size, to get acceptable results.
void testApp::activateClosestQuad(ofPoint point)
{
    float smallestDistance = 1.0;
    int closestQuad = activeQuad;

    for(int i = 0; i < 36; i++)
    {
        if (quads[i].initialized)
        {
            // find the quad closest to the point
            const float distanceX = quads[i].center.x - static_cast<float>(point.x) / ofGetWidth();
            const float distanceY = quads[i].center.y - static_cast<float>(point.y) / ofGetHeight();
            const float distance  = distanceX * distanceX + distanceY * distanceY; // no square root needed, since we can simply square the value it's compared to

            if(distance < smallestDistance && distance < 0.01)
            {
                closestQuad = i;
                smallestDistance = distance;
            }
        }
    }
    // if the closest quad is not currently active, activate it
    if (closestQuad != activeQuad)
    {
        quads[activeQuad].isActive = false;
        activeQuad = closestQuad;
        quads[activeQuad].isActive = true;
        m_gui.updatePages(quads[activeQuad]);
    }
}


// let the user choose an .xml project file with all the quads settings and loads it
void testApp::loadProject()
{
    ofFileDialogResult dialogResult = ofSystemLoadDialog("Load project file (.xml)");

    if(dialogResult.bSuccess)
    {
        loadSettingsFromXMLFile(dialogResult.getPath());
        m_gui.updatePages(quads[activeQuad]);
        m_gui.showPage(2);
    }
}


// saves quads settings to an .xml project file in data directory
void testApp::saveProject()
{
    ofFileDialogResult dialog_result = ofSystemSaveDialog("lpmt_settings.xml", "Save project file (.xml)");

    if(dialog_result.bSuccess)
    {
        saveCurrentSettingsToXMLFile(dialog_result.getPath());
    }
}
