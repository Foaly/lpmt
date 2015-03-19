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
    m_loadProjectFlag = false;
    m_saveProjectFlag = false;
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


    // defines the first 4 default quads
    m_quads.reserve(4);

    #ifdef WITH_KINECT
        #ifdef WITH_SYPHON
        Quad quadOne(ofPoint(0.0, 0.0), ofPoint(0.5, 0.0), ofPoint(0.5, 0.5), ofPoint(0.0, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
        #else
        Quad quadOne(ofPoint(0.0, 0.0), ofPoint(0.5, 0.0), ofPoint(0.5, 0.5), ofPoint(0.0, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
        #endif
    #else
        #ifdef WITH_SYPHON
        Quad quadOne(ofPoint(0.0, 0.0), ofPoint(0.5, 0.0), ofPoint(0.5, 0.5), ofPoint(0.0, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
        #else
        Quad quadOne(ofPoint(0.0, 0.0), ofPoint(0.5, 0.0), ofPoint(0.5, 0.5), ofPoint(0.0, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
        #endif
    #endif
    quadOne.quadNumber = 0;
    m_quads.push_back(quadOne);

    #ifdef WITH_KINECT
        #ifdef WITH_SYPHON
        Quad quadTwo(ofPoint(0.5, 0.0), ofPoint(1.0, 0.0), ofPoint(1.0, 0.5), ofPoint(0.5, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
        #else
        Quad quadTwo(ofPoint(0.5, 0.0), ofPoint(1.0, 0.0), ofPoint(1.0, 0.5), ofPoint(0.5, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
        #endif
    #else
        #ifdef WITH_SYPHON
        Quad quadTwo(ofPoint(0.5, 0.0), ofPoint(1.0, 0.0), ofPoint(1.0, 0.5), ofPoint(0.5, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
        #else
        Quad quadTwo(ofPoint(0.5, 0.0), ofPoint(1.0, 0.0), ofPoint(1.0, 0.5), ofPoint(0.5, 0.5), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
        #endif
    #endif
    quadTwo.quadNumber = 1;
    m_quads.push_back(quadTwo);

    #ifdef WITH_KINECT
        #ifdef WITH_SYPHON
        Quad quadThree(ofPoint(0.0, 0.5), ofPoint(0.5, 0.5), ofPoint(0.5, 1.0), ofPoint(0.0, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
        #else
        Quad quadThree(ofPoint(0.0, 0.5), ofPoint(0.5, 0.5), ofPoint(0.5, 1.0), ofPoint(0.0, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
        #endif
    #else
        #ifdef WITH_SYPHON
        Quad quadThree(ofPoint(0.0, 0.5), ofPoint(0.5, 0.5), ofPoint(0.5, 1.0), ofPoint(0.0, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
        #else
        Quad quadThree(ofPoint(0.0, 0.5), ofPoint(0.5, 0.5), ofPoint(0.5, 1.0), ofPoint(0.0, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
        #endif
    #endif
    quadThree.quadNumber = 2;
    m_quads.push_back(quadThree);

    #ifdef WITH_KINECT
        #ifdef WITH_SYPHON
        Quad quadFour(ofPoint(0.5, 0.5), ofPoint(1.0, 0.5) ,ofPoint(1.0, 1.0), ofPoint(0.5, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
        #else
        Quad quadFour(ofPoint(0.5, 0.5), ofPoint(1.0, 0.5) ,ofPoint(1.0, 1.0), ofPoint(0.5, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
        #endif
    #else
        #ifdef WITH_SYPHON
        Quad quadFour(ofPoint(0.5, 0.5), ofPoint(1.0, 0.5) ,ofPoint(1.0, 1.0), ofPoint(0.5, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
        #else
        Quad quadFour(ofPoint(0.5, 0.5), ofPoint(1.0, 0.5) ,ofPoint(1.0, 1.0), ofPoint(0.5, 1.0), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
        #endif
    #endif
    quadFour.quadNumber = 3;
    m_quads.push_back(quadFour);

    // define last one as active quad
    m_activeQuad = m_quads.end() - 1;
    m_activeQuad->isActive = true;

    // timeline stuff initialization
    #ifdef WITH_TIMELINE
    timelineSetup(timelineDurationSeconds);
    #endif

    // GUI STUFF ---------------------------------------------------
    m_gui.setupPages();
    m_gui.updatePages(*m_activeQuad);
    m_gui.showPage(2);

    // timeline off at start
    bTimeline = false;
    #ifdef WITH_TIMELINE
    timeline.setCurrentPage(ofToString(m_activeQuad->quadNumber));
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
        m_gui.updatePages(*m_activeQuad);

        isSetup = false;
        m_gui.hide();
        bGui = false;
        for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
        {
            quad->isSetup = false;
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
            m_activeQuad->resetDimensions();
            m_activeQuad->resetPlacement();
        }

        //check if quad bezier spherize button in the GUI was pressed
        if(m_bezierSpherizeQuadFlag)
        {
            m_bezierSpherizeQuadFlag = false;
            m_activeQuad->bezierSpherize();
        }

        //check if quad bezier spherize strong button in the GUI was pressed
        if(m_bezierSpherizeQuadStrongFlag)
        {
            m_bezierSpherizeQuadStrongFlag = false;
            m_activeQuad->bezierSpherizeStrong();
        }

        //check if quad bezier reset button in the GUI was pressed
        if(m_bezierResetQuadFlag)
        {
            m_bezierResetQuadFlag = false;
            m_activeQuad->resetBezier();
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
        for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
        {
            quad->update();
            // frame delay correction for Mpe sync
            if(bMpe)
            {
                if(quad->videoBg && quad->video.isLoaded())
                {
                    int mpeFrame = client.getFrameCount();
                    int totFrames = quad->video.getTotalNumFrames();
                    int videoFrame = quad->video.getCurrentFrame();
                    //quad->video.setFrame(mpeFrame%totFrames);
                    if(abs((mpeFrame%totFrames) - videoFrame) > 2) // TODO: testing different values
                    {
                        //cout << mpeFrame%totFrames << endl;
                        quad->video.setFrame(mpeFrame%totFrames);
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
        for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
        {
            quad->draw();
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
            ttf.drawString("active surface: " + ofToString(m_activeQuad->quadNumber), 30, ofGetHeight() - 25);

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
    // moves active quad one position up
    if ( key == '+' && !bTimeline && !bGui)
    {
        if(m_activeQuad + 1 < m_quads.end())
        {
            // TODO: remove console error
            iter_swap(m_activeQuad, m_activeQuad + 1);
            m_activeQuad++;
        }
    }


    // moves active quad one position down
    if ( key == '-' && !bTimeline && !bGui)
    {
        if(m_activeQuad > m_quads.begin())
        {
            iter_swap(m_activeQuad, m_activeQuad - 1);
            m_activeQuad--;
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
            m_activeQuad->corners[0] = ofPoint(0.0, 0.0);
            m_activeQuad->corners[1] = ofPoint(1.0, 0.0);
            m_activeQuad->corners[2] = ofPoint(1.0, 1.0);
            m_activeQuad->corners[3] = ofPoint(0.0, 1.0);
        }
    }

    // activates next quad
    if ( key =='>' && !bTimeline)
    {
        if (isSetup)
        {
            m_activeQuad->isActive = false;
            m_activeQuad++;
            if(m_activeQuad == m_quads.end())
                m_activeQuad = m_quads.begin();

            m_activeQuad->isActive = true;
        }
        m_gui.updatePages(*m_activeQuad);
    }

    // activates prev quad
    if ( key =='<' && !bTimeline)
    {
        if (isSetup)
        {
            m_activeQuad->isActive = false;
            m_activeQuad--;
            if(m_activeQuad < m_quads.begin())
                m_activeQuad = m_quads.end() - 1;

            m_activeQuad->isActive = true;
        }
        m_gui.updatePages(*m_activeQuad);
    }

    // goes to first page of gui for active quad or, in mask edit mode, delete last drawn point
    if ( (key == 'z' || key == 'Z') && !bTimeline)
    {
        if(maskSetup && m_activeQuad->m_maskPoints.size() > 0)
        {
            m_activeQuad->m_maskPoints.pop_back();
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

    // when in mask edit mode remove the highlighted mask point
    if ( (key == 'd' || key == 'D') && !bTimeline)
    {
        if(maskSetup && m_activeQuad->m_maskPoints.size() > 0)
        {
            if (m_activeQuad->bHighlightMaskPoint)
            {
                m_activeQuad->m_maskPoints.erase(m_activeQuad->m_maskPoints.begin() + m_activeQuad->highlightedMaskPoint);
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
            m_activeQuad->m_maskPoints.clear();
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
        // TODO use a copy constructor for this!
        //m_sourceQuadForCopying = activeQuad;
    }

    // paste settings from source surface to currently active surface
    // (22 corresponds to CTRL + V)
    if ( (key == 22) && !bTimeline)
    {
//        copyQuadSettings(m_sourceQuadForCopying);
    }

    // adds a new quad in the middle of the screen
    if ( key =='a' && !bTimeline)
    {
        if (isSetup)
        {
            #ifdef WITH_KINECT
                #ifdef WITH_SYPHON
                Quad quad(ofPoint(0.25, 0.25), ofPoint(0.75, 0.25), ofPoint(0.75, 0.75), ofPoint(0.25, 0.75), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
                #else
                Quad quad(ofPoint(0.25, 0.25), ofPoint(0.75, 0.25), ofPoint(0.75, 0.75), ofPoint(0.25, 0.75), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
                #endif
            #else
                #ifdef WITH_SYPHON
                Quad quad(ofPoint(0.25, 0.25), ofPoint(0.75, 0.25), ofPoint(0.75, 0.75), ofPoint(0.25, 0.75), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
                #else
                Quad quad(ofPoint(0.25, 0.25), ofPoint(0.75, 0.25), ofPoint(0.75, 0.75), ofPoint(0.25, 0.75), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
                #endif
            #endif
            quad.quadNumber = m_quads.size();

            m_activeQuad->isActive = false;
            quad.isActive = true;

            m_quads.push_back(quad);
            m_activeQuad = m_quads.end() - 1;

            m_gui.updatePages(*m_activeQuad);

            // add timeline page for new quad
            #ifdef WITH_TIMELINE
            timelineAddQuadPage(m_activeQuad->quadNumber);
            #endif
            // next line fixes a bug i've been tracking down for a looong time
            glDisable(GL_DEPTH_TEST);
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
            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                quad->isSetup = false;
            }
        }
        else
        {
            isSetup = true;
            m_gui.show();
            bGui = true;
            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                quad->isSetup = true;
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
        if (maskSetup)
        {
            maskSetup = false;
            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                quad->isMaskSetup = false;
            }

        }
        m_gui.toggleDraw();
        bGui = !bGui;
    }

    // toggles mask editing
    if(key == 'm' && !bTimeline)
    {
        if (!bGui)
        {
            maskSetup = !maskSetup;
            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                quad->isMaskSetup = !quad->isMaskSetup;
            }
        }
    }

    // toggles bezier deformation editing
    if(key == 'b' && !bTimeline)
    {
        if (!bGui)
        {
            gridSetup = !gridSetup;
            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                quad->isBezierSetup = !quad->isBezierSetup;
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
            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                quad->isOn = false;
                if (quad->videoBg && quad->video.isLoaded())
                {
                    quad->video.setVolume(0);
                    quad->video.stop();
                }
            }
        }
        else if(!bStarted)
        {
            bStarted = true;
            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                quad->isOn = true;
                if (quad->videoBg && quad->video.isLoaded())
                {
                    quad->video.setVolume(quad->videoVolume);
                    quad->video.play();
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
        if(m_cameras[m_activeQuad->camNumber].getPixelFormat() == OF_PIXELS_RGBA)
        {
            m_cameras[m_activeQuad->camNumber].setPixelFormat(OF_PIXELS_BGRA);
        }
        else if(m_cameras[m_activeQuad->camNumber].getPixelFormat() == OF_PIXELS_BGRA)
        {
            m_cameras[m_activeQuad->camNumber].setPixelFormat(OF_PIXELS_RGBA);
        }

    }

    // rotation of surface around its center
    if(key == '£' && !bTimeline)
    {
        ofMatrix4x4 rotation;
        ofMatrix4x4 centerToOrigin;
        ofMatrix4x4 originToCenter;
        ofMatrix4x4 resultingMatrix;
        centerToOrigin.makeTranslationMatrix(-m_activeQuad->center);
        originToCenter.makeTranslationMatrix(m_activeQuad->center);
        rotation.makeRotationMatrix(-5.0,0,0,1);
        resultingMatrix = centerToOrigin * rotation * originToCenter;
        for(int i=0; i<4; i++)
        {
            m_activeQuad->corners[i] = m_activeQuad->corners[i] * resultingMatrix;
        }
    }

    if(key == '$' && !bTimeline)
    {
        ofMatrix4x4 rotation;
        ofMatrix4x4 centerToOrigin;
        ofMatrix4x4 originToCenter;
        ofMatrix4x4 resultingMatrix;
        centerToOrigin.makeTranslationMatrix(-m_activeQuad->center);
        originToCenter.makeTranslationMatrix(m_activeQuad->center);
        rotation.makeRotationMatrix(5.0,0,0,1);
        resultingMatrix = centerToOrigin * rotation * originToCenter;
        for(int i=0; i<4; i++)
        {
            m_activeQuad->corners[i] = m_activeQuad->corners[i] * resultingMatrix;
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
            float distx = m_activeQuad->corners[i].x - (float)x/ofGetWidth();
            float disty = m_activeQuad->corners[i].y - (float)y/ofGetHeight();
            float dist  = sqrt( distx * distx + disty * disty);

            if(dist < smallestDist && dist < 0.05) // value for dist threshold can vary between 0.1-0.05, fine tune it
            {
                m_selectedCorner = i;
                smallestDist = dist;
            }
        }

        if(m_selectedCorner >= 0)
        {
            m_activeQuad->bHighlightCorner = true;
            m_activeQuad->highlightedCorner = m_selectedCorner;
        }
        else
        {
            m_activeQuad->bHighlightCorner = false;
            m_activeQuad->highlightedCorner = -1;

            // distance from center
            float distx = m_activeQuad->center.x - (float)x / ofGetWidth();
            float disty = m_activeQuad->center.y - (float)y/ofGetHeight();
            float dist  = sqrt( distx * distx + disty * disty);
            if(dist < 0.05)
            {
                m_activeQuad->bHighlightCenter = true;
            }
            else
            {
                m_activeQuad->bHighlightCenter = false;
            }

            // distance from rotation grab point
            ofPoint rotationGrabPoint;
            //rotationGrabPoint.x = (quads[activeQuad].corners[2].x - quads[activeQuad].corners[1].x)/2 + quads[activeQuad].corners[1].x;
            //rotationGrabPoint.y = (quads[activeQuad].corners[2].y - quads[activeQuad].corners[1].y)/2 + quads[activeQuad].corners[1].y;
            //rotationGrabPoint = ((quads[activeQuad].corners[2]+quads[activeQuad].corners[1])/2+quads[activeQuad].center)/2;
            rotationGrabPoint = (m_activeQuad->center);
            rotationGrabPoint.x = rotationGrabPoint.x + 0.1;
            float rotationDistx = rotationGrabPoint.x - (float)x / ofGetWidth();
            float rotationDisty = rotationGrabPoint.y - (float)y / ofGetHeight();
            float rotationDist = sqrt(rotationDistx * rotationDistx + rotationDisty * rotationDisty);
            if(rotationDist < 0.05)
            {
                m_activeQuad->bHighlightRotation = true;
            }
            else
            {
                m_activeQuad->bHighlightRotation = false;
            }
        }
    }

    else if (maskSetup && !gridSetup && !bTimeline)
    {
        float smallestDist = sqrt( ofGetWidth() * ofGetWidth() + ofGetHeight() * ofGetHeight());;
        int whichPoint = -1;
        ofVec3f warped;
        for(int i = 0; i < m_activeQuad->m_maskPoints.size(); i++)
        {
            warped = m_activeQuad->getWarpedPoint(mousePosition);
            float distx = (float)m_activeQuad->m_maskPoints[i].x * ofGetWidth() - (float)warped.x;
            float disty = (float)m_activeQuad->m_maskPoints[i].y * ofGetHeight()- (float)warped.y;
            float dist  = sqrt( distx * distx + disty * disty);

            if(dist < smallestDist && dist < 20.0)
            {
                whichPoint = i;
                smallestDist = dist;
            }
        }
        if(whichPoint >= 0)
        {
            m_activeQuad->bHighlightMaskPoint = true;
            m_activeQuad->highlightedMaskPoint = whichPoint;
        }
        else
        {
            m_activeQuad->bHighlightMaskPoint = false;
            m_activeQuad->highlightedMaskPoint = -1;
        }
    }

    else if (gridSetup && !maskSetup && !bTimeline)
    {
        float smallestDist = sqrt( ofGetWidth() * ofGetWidth() + ofGetHeight() * ofGetHeight());;
        int whichPointRow = -1;
        int whichPointCol = -1;
        ofVec3f warped;

        if(m_activeQuad->bBezier)
        {
        for(int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                warped = m_activeQuad->getWarpedPoint(mousePosition);
                float distx = (float)m_activeQuad->bezierPoints[i][j][0] * ofGetWidth() - (float)warped.x;
                float disty = (float)m_activeQuad->bezierPoints[i][j][1] * ofGetHeight() - (float)warped.y;
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

        else if(m_activeQuad->bGrid)
        {
        for(int i = 0; i <= m_activeQuad->gridRows; i++)
        {
            for (int j = 0; j <= m_activeQuad->gridColumns; j++)
            {
                warped = m_activeQuad->getWarpedPoint(mousePosition);
                float distx = (float)m_activeQuad->gridPoints[i][j][0] * ofGetWidth() - (float)warped.x;
                float disty = (float)m_activeQuad->gridPoints[i][j][1] * ofGetHeight() - (float)warped.y;
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
            m_activeQuad->bHighlightCtrlPoint = true;
            m_activeQuad->highlightedCtrlPointRow = whichPointRow;
            m_activeQuad->highlightedCtrlPointCol = whichPointCol;
        }
        else
        {
            m_activeQuad->bHighlightCtrlPoint = false;
            m_activeQuad->highlightedCtrlPointRow = -1;
            m_activeQuad->highlightedCtrlPointCol = -1;
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
            m_activeQuad->corners[m_selectedCorner] = ofPoint(normalizedX, normalizedY);
        }
        else
        {
            // if no corner is selected, check if we can move or rotate whole quad
            //by dragging its center and rotation mark
            if(m_activeQuad->bHighlightCenter) // TODO: verifiy if threshold value is good for distance
            {
                ofPoint movement(mousePosition - startDrag);

                // move the entire quad
                for(int i = 0; i < 4; i++)
                {
                    m_activeQuad->corners[i].x += ((float)movement.x / ofGetWidth());
                    m_activeQuad->corners[i].y += ((float)movement.y / ofGetHeight());
                }
                startDrag = mousePosition;
            }
            // rotate the quad
            else if(m_activeQuad->bHighlightRotation)
            {
                float angle;
                // quad center in pixel coordinates
                ofPoint center(m_activeQuad->center.x * ofGetWidth(), m_activeQuad->center.y * ofGetHeight());
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
                centerToOrigin.makeTranslationMatrix(-m_activeQuad->center);
                originToCenter.makeTranslationMatrix(m_activeQuad->center);
                rotation.makeRotationMatrix(angle, 0, 0, 1);
                resultingMatrix = centerToOrigin * rotation * originToCenter;
                for(int i=0; i<4; i++)
                {
                    m_activeQuad->corners[i] = m_activeQuad->corners[i] * resultingMatrix;
                }
                startDrag = mousePosition;
            }
        }
    }
    else if(maskSetup && m_activeQuad->bHighlightMaskPoint && !bTimeline)
    {
        // in mask setup mode, move the selected mask point
        const ofPoint warpedPoint = m_activeQuad->getWarpedPoint(mousePosition);
        const ofPoint normalizedPoint(warpedPoint.x / ofGetWidth(), warpedPoint.y / ofGetHeight());

        m_activeQuad->m_maskPoints[m_activeQuad->highlightedMaskPoint] = normalizedPoint;
    }

    else if(gridSetup && m_activeQuad->bHighlightCtrlPoint && !bTimeline)
    {
        const int currentRow = m_activeQuad->highlightedCtrlPointRow;
        const int currentCol = m_activeQuad->highlightedCtrlPointCol;
        const ofPoint warpedPoint = m_activeQuad->getWarpedPoint(mousePosition);

        if(m_activeQuad->bBezier)
        {
            m_activeQuad->bezierPoints[currentRow][currentCol][0] = (float)warpedPoint.x / ofGetWidth();
            m_activeQuad->bezierPoints[currentRow][currentCol][1] = (float)warpedPoint.y / ofGetHeight();
        }
        else if(m_activeQuad->bGrid)
        {
            m_activeQuad->gridPoints[currentRow][currentCol][0] = (float)warpedPoint.x / ofGetWidth();
            m_activeQuad->gridPoints[currentRow][currentCol][1] = (float)warpedPoint.y / ofGetHeight();
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
            if (!m_activeQuad->bHighlightMaskPoint)
            {
                m_activeQuad->maskAddPoint(mousePosition);
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
                float distx = m_activeQuad->corners[i].x - (float)x/ofGetWidth();
                float disty = m_activeQuad->corners[i].y - (float)y/ofGetHeight();
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
            std::vector<Quad>::iterator snapQuad = m_quads.end();
            int snapCorner = -1;

            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                for(int j = 0; j < 4; j++)
                {
                    float distx = m_activeQuad->corners[m_selectedCorner].x - quad->corners[j].x;
                    float disty = m_activeQuad->corners[m_selectedCorner].y - quad->corners[j].y;
                    float dist = std::sqrt( distx * distx + disty * disty);
                    // to tune snapping change dist value inside next if statement
                    // TODO: check out the value
                    if (dist < smallestDist && dist < 0.0075)
                    {
                        snapQuad = quad;
                        snapCorner = j;
                        smallestDist = dist;
                    }
                }
            }
            if (snapQuad != m_quads.end() && snapCorner >= 0 && bSnapOn)
            {
                m_activeQuad->corners[m_selectedCorner].x = snapQuad->corners[snapCorner].x;
                m_activeQuad->corners[m_selectedCorner].y = snapQuad->corners[snapCorner].y;
            }
        }
        m_selectedCorner = -1;
        m_activeQuad->bHighlightCorner = false;
    }
}


void testApp::windowResized(int w, int h)
{
    #ifdef WITH_TIMELINE
    timeline.setWidth(w);
    #endif
    for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
    {
        quad->bHighlightCorner = false;
        quad->allocateFbo(ofGetWidth(),ofGetHeight());
        quad->resetDimensions();
    }
}


//---------------------------------------------------------------
// This method activates the quad, whos center is closest to the given point.
// Since we don't have a proper quad/point intersection test, the point must also be
// within a radius the size of a 10th of the windows size, to get acceptable results.
void testApp::activateClosestQuad(ofPoint point)
{
    float smallestDistance = 1.0;
    std::vector<Quad>::iterator closestQuad = m_quads.end();

    for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
    {
        // find the quad closest to the point
        const float distanceX = quad->center.x - static_cast<float>(point.x) / ofGetWidth();
        const float distanceY = quad->center.y - static_cast<float>(point.y) / ofGetHeight();
        const float distance  = distanceX * distanceX + distanceY * distanceY; // no square root needed, since we can simply square the value it's compared to

        if(distance < smallestDistance && distance < 0.01)
        {
            closestQuad = quad;
            smallestDistance = distance;
        }
    }
    // if the closest quad is not currently active, activate it
    if (closestQuad != m_activeQuad && closestQuad != m_quads.end())
    {
        m_activeQuad->isActive = false;
        m_activeQuad = closestQuad;
        m_activeQuad->isActive = true;
        m_gui.updatePages(*m_activeQuad);
    }
}


// let the user choose an .xml project file with all the quads settings and loads it
void testApp::loadProject()
{
    ofFileDialogResult dialogResult = ofSystemLoadDialog("Load project file (.xml)");

    if(dialogResult.bSuccess)
    {
        loadSettingsFromXMLFile(dialogResult.getPath());
        m_gui.updatePages(*m_activeQuad);
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
