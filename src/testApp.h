#ifndef _TEST_APP
#define _TEST_APP

#include "config.h"

#define OF_ADDON_USING_OFXXMLSETTINGS

//#define WINDOW_W 800
//#define WINDOW_H 600

// OSC stuff - listen on port 12345
#define PORT 12345
#define NUM_MSG_STRINGS 20

#include "ofMain.h"

#include "quad.h"
#ifdef WITH_KINECT
#include "kinectManager.h"
#endif
#include "ofxXmlSettings.h"
#include "ofxMostPixelsEver.h"
#include "ofxOsc.h"
#ifdef WITH_TIMELINE
#include "ofxTimeline.h"
#endif
#include "ofxSimpleGuiToo.h"
#ifdef WITH_SYPHON
#include "ofxSyphon.h"
#endif
#ifdef WITH_MIDI
#include "ofxMidi.h"
#endif

//#include <GL/glut.h>

#ifdef WITH_MIDI
class testApp : public ofBaseApp, public ofxMidiListener
#else
class testApp : public ofBaseApp
#endif
{

public:

    int WINDOW_W;
    int WINDOW_H;

    void setup();
    void update();
    void draw();
    void prepare();
    void dostuff();
    void exit();

    void mpeSetup();
    void resync();
    void startProjection();
    void stopProjection();
    void copyQuadSettings(int sourceQuad);


    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased();
    void windowResized(int w, int h);

    int m_selectedCorner;

    ofTrueTypeFont ttf;

    quad quads[36];
    int layers[36];

    int activeQuad;
    int nOfQuads;
    int m_sourceQuadForCopying;

    bool autoStart;

    bool isSetup;
    bool bFullscreen;
    bool bGui;
    bool bTimeline;
    bool bStarted;
    bool maskSetup;
    bool gridSetup;
    bool bSnapOn;

    // splash screen variables
    bool m_isSplashScreenActive;
    ofImage m_SplashScreenImage;

    // use of MostPixelsEver
    bool bMpe;
    ofxMPEClient client;
    void mpeFrameEvent(ofxMPEEventArgs& event);
    void mpeMessageEvent(ofxMPEEventArgs& event);
    void mpeResetEvent(ofxMPEEventArgs& event);
    float lastFrameTime;

    // OSC stuff
    ofxOscReceiver	receiver;
    int				current_msg_string;
    string		msg_strings[NUM_MSG_STRINGS];
    float			timers[NUM_MSG_STRINGS];
    void parseOsc();
    float oscControlMin;
    float oscControlMax;
    vector<ofxOscMessage> oscHotkeyMessages;
    vector<int> oscHotkeyKeys;

    // MIDI stuff
    #ifdef WITH_MIDI
    void newMidiMessage(ofxMidiMessage& eventArgs);
    ofxMidiIn midiIn;
	ofxMidiMessage midiMessage;
	vector<ofxMidiMessage> midiHotkeyMessages;
	vector<int> midiHotkeyKeys;
	#endif

    // Shaders
    ofShader edgeBlendShader;
    ofShader quadMaskShader;
    ofShader chromaShader;

    // gui elements
    bool showGui;

    bool bImageLoad;
    bool bVideoLoad;
    bool bSharedVideoLoad0;
    bool bSharedVideoLoad1;
    bool bSharedVideoLoad2;
    bool bSharedVideoLoad3;
    bool bSharedVideoLoad4;
    bool bSharedVideoLoad5;
    bool bSharedVideoLoad6;
    bool bSharedVideoLoad7;
    bool bQuadReset;
    bool bQuadBezierSpherize;
    bool bQuadBezierSpherizeStrong;
    bool bQuadBezierReset;
    bool bMidiHotkeyCoupling;
    bool bMidiHotkeyLearning;
    int midiHotkeyPressed;

    void openImageFile();
    void openVideoFile();
    void openSharedVideoFile(int i);
    void openSharedVideoFile(string path, int i);
    void quadDimensionsReset(int q);
    void quadPlacementReset(int q);
    void quadBezierSpherize(int q);
    void quadBezierSpherizeStrong(int q);
    void quadBezierReset(int q);

    float totRotationAngle;
    ofPolyline rotationSector;

    // snapshot background variables
    bool m_isSnapshotTextureOn;
    ofTexture m_snapshotBackgroundTexture;
	std::vector<ofVideoGrabber> m_cameras;
	std::vector<ofVideoGrabber>::iterator m_snapshotBackgroundCamera;
	std::vector<string> m_cameraIds;

    vector<ofVideoPlayer> sharedVideos;
    vector<string> sharedVideosFiles;

    vector<string> imgFiles;
    vector<string> videoFiles;
    vector<string> slideshowFolders;

    void saveCurrentSettingsToXMLFile(std::string xmlFilePath);
    void loadSettingsFromXMLFile(std::string xmlFilePath);

    // double-click variables for quad picking
    unsigned long m_doubleclickTime;
    unsigned long long m_timeLastClicked;

    void activateClosestQuad(ofPoint point);

    ofPoint startDrag;


    // snapshot loading
    ofImage loadImageFromFile();

    string loadSlideshow();
    bool bSlideshowLoad;

    #ifdef WITH_KINECT
    kinectManager kinect;
    bool m_isKinectInitialized;
    bool bCloseKinect;
    bool bOpenKinect;
    #endif

    // timeline
    #ifdef WITH_TIMELINE
    ofxTimeline timeline;
    float timelineDurationSeconds;
    float timelinePreviousDuration;
    void timelineSetup(float duration);
    void timelineUpdate();
    void timelineAddQuadPage(int i);
    void timelineTriggerReceived(ofxTLBangEventArgs& trigger);
    bool useTimeline;
    #endif

    // syphon
    #ifdef WITH_SYPHON
	ofxSyphonClient syphClient;
    #endif


};

#endif

