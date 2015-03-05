#include "GUI.hpp"
#include "testApp.h"

GUI::GUI(testApp* app) : m_app(app),
                         m_dummyBool(false),
                         m_dummyFloat(0.f),
                         m_dummyInt(0)
{
    // replace default config with this. That requires some change in the lib tough
//    // overriding default theme
//    m_config.toggleHeight = 16;
//    m_config.buttonHeight = 18;
//    m_config.sliderTextHeight = 18;
//    m_config.titleHeight = 18;
//
//    m_config.fullActiveColor = 0x648B96; // bluish-grey
//    m_config.textColor = 0xFFFFFF;       // white
//    m_config.textBGOverColor = 0xDB6800; // orange
//
//    gui.config = &m_config;

    // initializing gui (sets up header page etc.)
    gui.setup();

    defaultSimpleGuiConfig.toggleHeight = 16;
    defaultSimpleGuiConfig.buttonHeight = 18;
    defaultSimpleGuiConfig.sliderTextHeight = 18;
    defaultSimpleGuiConfig.titleHeight = 18;

    defaultSimpleGuiConfig.fullActiveColor = 0x648B96;
    defaultSimpleGuiConfig.textColor = 0xFFFFFF;
    defaultSimpleGuiConfig.textBGOverColor = 0xDB6800;
}

/**
 * This method sets up the GUI pages structure and initializes the quad specific values with dummies.
 * The loading flags are set here as well.
 * The data binding to the currently selected quad happens in updatePages()
 */
void GUI::setupPages()
{
    // general page
    gui.addTitle("show/hide quads");
    // first the default page for general controls and toggling surfaces on/off
    for(int i = 0; i < 36; i++)
    {
        gui.addToggle("surface " + ofToString(i), m_app->quads[i].isOn);
    }
    gui.addTitle("General controls").setNewColumn(true);
    gui.addToggle("surfaces corner snap", m_app->bSnapOn);
    gui.addTitle("Shared videos");
    gui.addButton("load shared video 1", m_app->m_loadSharedVideo0Flag);
    gui.addButton("load shared video 2", m_app->m_loadSharedVideo1Flag);
    gui.addButton("load shared video 3", m_app->m_loadSharedVideo2Flag);
    gui.addButton("load shared video 4", m_app->m_loadSharedVideo3Flag);
    gui.addButton("load shared video 5", m_app->m_loadSharedVideo4Flag);
    gui.addButton("load shared video 6", m_app->m_loadSharedVideo5Flag);
    gui.addButton("load shared video 7", m_app->m_loadSharedVideo6Flag);
    gui.addButton("load shared video 8", m_app->m_loadSharedVideo7Flag);
    #ifdef WITH_TIMELINE
    gui.addTitle("Timeline");
    gui.addToggle("use timeline", m_app->useTimeline);
    gui.addSlider("timeline seconds", m_app->timelineDurationSeconds, 10.0, 1200.0);
    #endif

    // Page One
    gui.addPage("Page 1");
    gui.addTitle("surface");
    gui.addToggle("show/hide", m_dummyBool);
    #ifdef WITH_TIMELINE
    gui.addToggle("use timeline", m_app->useTimeline);
    gui.addSlider("timeline seconds", m_app->timelineDurationSeconds, 10.0, 1200.0);
    gui.addToggle("use timeline tint", m_dummyBool);
    gui.addToggle("use timeline color", m_dummyBool);
    gui.addToggle("use timeline alpha", m_dummyBool);
    gui.addToggle("use timeline for slides", m_dummyBool);
    #endif
    #ifdef WITH_SYPHON
    gui.addToggle("use Syphon", m_dummyBool);
    gui.addSlider("syphon origin X", m_dummyFloat, -1600, 1600);
    gui.addSlider("syphon origin Y", m_dummyFloat, -1600, 1600);
    gui.addSlider("syphon scale X", m_dummyFloat, 0.1, 10.0);
    gui.addSlider("syphon scale Y", m_dummyFloat, 0.1, 10.0);
    #endif
    gui.addToggle("image on/off", m_dummyBool);
    gui.addButton("load image", m_app->m_loadImageFlag);
    gui.addSlider("img scale X", m_dummyFloat, 0.1, 10.0);
    gui.addSlider("img scale Y", m_dummyFloat, 0.1, 10.0);
    gui.addToggle("H mirror", m_dummyBool);
    gui.addToggle("V mirror", m_dummyBool);
    gui.addColorPicker("img color", &m_dummyFloat);
    gui.addTitle("Blending modes");
    gui.addToggle("blending on/off", m_dummyBool);
    std::string blendModesArray[] = {"None", "Normal Alpha-Blending", "Additive (with Alpha)", "Subtractive (with Alpha)", "Multiply", "Screen"};
    gui.addComboBox("blending mode", m_dummyInt, 6, blendModesArray);

    gui.addTitle("Solid color").setNewColumn(true);
    gui.addToggle("solid bg on/off", m_dummyBool);
    gui.addColorPicker("Color", &m_dummyFloat);
    gui.addToggle("transition color",m_dummyBool);
    gui.addColorPicker("second Color", &m_dummyFloat);
    gui.addSlider("trans duration", m_dummyFloat, 0.1, 60.0);
    gui.addTitle("Mask");
    gui.addToggle("mask on/off", m_dummyBool);
    gui.addToggle("invert mask", m_dummyBool);
    gui.addTitle("Surface deformation");
    gui.addToggle("surface deform.", m_dummyBool);
    gui.addToggle("use bezier", m_dummyBool);
    gui.addToggle("use grid", m_dummyBool);
    gui.addSlider("grid rows", m_dummyInt, 2, 15);
    gui.addSlider("grid columns", m_dummyInt, 2, 20);
    gui.addButton("spherize light", m_app->bQuadBezierSpherize);
    gui.addButton("spherize strong", m_app->bQuadBezierSpherizeStrong);
    gui.addButton("reset bezier warp", m_app->bQuadBezierReset);

    gui.addTitle("Edge blending").setNewColumn(true);
    gui.addToggle("edge blend on/off", m_dummyBool);
    gui.addSlider("power", m_dummyFloat, 0.0, 4.0);
    gui.addSlider("gamma", m_dummyFloat, 0.0, 4.0);
    gui.addSlider("luminance", m_dummyFloat, -4.0, 4.0);
    gui.addSlider("left edge", m_dummyFloat, 0.0, 0.5);
    gui.addSlider("right edge", m_dummyFloat, 0.0, 0.5);
    gui.addSlider("top edge", m_dummyFloat, 0.0, 0.5);
    gui.addSlider("bottom edge", m_dummyFloat, 0.0, 0.5);
    gui.addTitle("Content placement");
    gui.addSlider("X displacement", m_dummyInt, -1600, 1600);
    gui.addSlider("Y displacement", m_dummyInt, -1600, 1600);
    gui.addSlider("Width", m_dummyInt, 0, 2400);
    gui.addSlider("Height", m_dummyInt, 0, 2400);
    gui.addButton("Reset", m_app->m_resetCurrentQuadFlag);

    // Page Two
    gui.addPage("Page 2");
    gui.addTitle("Video");
    gui.addToggle("video on/off", m_dummyBool);
    //gui.addComboBox("video bg", quads[i].bgVideo, videoFiles.size(), videos);
    gui.addButton("load video", m_app->m_loadVideoFlag);
    gui.addSlider("video scale X", m_dummyFloat, 0.1, 10.0);
    gui.addSlider("video scale Y", m_dummyFloat, 0.1, 10.0);
    gui.addToggle("H mirror", m_dummyBool);
    gui.addToggle("V mirror", m_dummyBool);
    gui.addColorPicker("video color", &m_dummyFloat);
    gui.addSlider("video sound vol", m_dummyInt, 0, 10);
    gui.addSlider("video speed", m_dummyFloat, -2.0, 4.0);
    gui.addToggle("video loop", m_dummyBool);
    gui.addToggle("video greenscreen", m_dummyBool);
    gui.addToggle("shared video on/off", m_dummyBool);
    gui.addSlider("shared video", m_dummyInt, 1, 8);

    if (m_app->m_cameras.size() > 0)
    {
        gui.addTitle("Camera").setNewColumn(true);
        gui.addToggle("cam on/off", m_dummyBool);
        if(m_app->m_cameras.size() > 1)
        {
            gui.addComboBox("select camera", m_dummyInt, m_app->m_cameras.size(), &(m_app->m_cameraIds[0]));
        }
        gui.addSlider("camera scale X", m_dummyFloat, 0.1, 10.0);
        gui.addSlider("camera scale Y", m_dummyFloat, 0.1, 10.0);
        gui.addToggle("flip H", m_dummyBool);
        gui.addToggle("flip V", m_dummyBool);
        gui.addColorPicker("cam color", &m_dummyFloat);
        gui.addToggle("camera greenscreen", m_dummyBool);
        gui.addTitle("Greenscreen");
    }
    else
    {
    gui.addTitle("Greenscreen").setNewColumn(true);
    }
    gui.addSlider("g-screen threshold", m_dummyFloat, 0.0, 255.0);
    gui.addColorPicker("greenscreen col", &m_dummyFloat);
    gui.addTitle("Slideshow").setNewColumn(false);
    gui.addToggle("slideshow on/off", m_dummyBool);
    gui.addButton("load slideshow", m_app->m_loadSlideshowFlag);
    gui.addSlider("slide duration", m_dummyFloat, 0.1, 15.0);
    gui.addToggle("slides to quad size", m_dummyBool);
    gui.addToggle("keep aspect ratio", m_dummyBool);

    #ifdef WITH_KINECT
    if(m_app->m_isKinectInitialized)
    {
        gui.addTitle("Kinect").setNewColumn(true);
        gui.addToggle("use kinect", m_dummyBool);
        gui.addToggle("show kinect image", m_dummyBool);
        gui.addToggle("show kinect gray image", m_dummyBool);
        gui.addToggle("use kinect as mask", m_dummyBool);
        gui.addToggle("kinect blob detection", m_dummyBool);
        gui.addToggle("blob curved contour", m_dummyBool);
        gui.addSlider("kinect scale X", m_dummyFloat, 0.1, 10.0);
        gui.addSlider("kinect scale Y", m_dummyFloat, 0.1, 10.0);
        gui.addColorPicker("kinect color", &m_dummyFloat);
        gui.addSlider("near threshold", m_dummyInt, 0, 255);
        gui.addSlider("far threshold", m_dummyInt, 0, 255);
        gui.addSlider("kinect tilt angle", m_app->kinect.kinectAngle, -30, 30);
        gui.addSlider("kinect image blur", m_dummyInt, 0, 10);
        gui.addSlider("blob min area", m_dummyFloat, 0.01, 1.0);
        gui.addSlider("blob max area", m_dummyFloat, 0.0, 1.0);
        gui.addSlider("blob contour smooth", m_dummyInt, 0, 20);
        gui.addSlider("blob simplify", m_dummyFloat, 0.0, 2.0);
        gui.addButton("close connection", m_dummyBool);
        gui.addButton("reopen connection", m_dummyBool);
    }
    #endif

    // Page Three
    gui.addPage("Page 3");
    gui.addTitle("Corner 0");
    gui.addSlider("corner 0 X", m_dummyFloat, -1.0, 2.0);
    gui.addSlider("corner 0 Y", m_dummyFloat, -1.0, 2.0);
    gui.addTitle("Corner 3");
    gui.addSlider("corner 3 X", m_dummyFloat, -1.0, 2.0);
    gui.addSlider("corner 3 Y", m_dummyFloat, -1.0, 2.0);

    gui.addTitle("Corner 1").setNewColumn(true);
    gui.addSlider("corner 1 X", m_dummyFloat, -1.0, 2.0);
    gui.addSlider("corner 1 Y", m_dummyFloat, -1.0, 2.0);
    gui.addTitle("Corner 2");
    gui.addSlider("corner 2 X", m_dummyFloat, -1.0, 2.0);
    gui.addSlider("corner 2 Y", m_dummyFloat, -1.0, 2.0);

    gui.addTitle("Crop").setNewColumn(true);
    gui.addToggle("mask on/off", m_dummyBool);
    gui.addTitle("Rectangular crop");
    gui.addSlider("top", m_dummyFloat, 0, 1.0);
    gui.addSlider("right", m_dummyFloat, 0, 1.0);
    gui.addSlider("bottom", m_dummyFloat, 0, 1.0);
    gui.addSlider("left", m_dummyFloat, 0, 1.0);
    gui.addTitle("Circular crop");
    gui.addSlider("center X", m_dummyFloat, 0, 1.0);
    gui.addSlider("center Y", m_dummyFloat, 0, 1.0);
    gui.addSlider("radius", m_dummyFloat, 0, 2.0);

    // then we set displayed gui page to the one corresponding to active quad and show the gui
//    gui.setPage((activeQuad*3)+2);
    gui.show();
}

void GUI::updatePages(quad& activeQuad)
{
    ofxSimpleGuiPage& firstPage = gui.page("Page 1");

    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("show/hide"))->value = &activeQuad.isOn;
    #ifdef WITH_TIMELINE
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("use timeline tint"))->value = &activeQuad.bTimelineTint;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("use timeline color"))->value = &activeQuad.bTimelineColor;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("use timeline alpha"))->value = &activeQuad.bTimelineAlpha;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("use timeline for slides"))->value = &activeQuad.bTimelineSlideChange;
    #endif
    #ifdef WITH_SYPHON
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("use Syphon"))->value = &activeQuad.bSyphon;
    dynamic_cast<ofxSimpleGuiSliderInt*>(firstPage.findControlByName("syphon origin X"))->value = &activeQuad.syphonPosX;
    dynamic_cast<ofxSimpleGuiSliderInt*>(firstPage.findControlByName("syphon origin Y"))->value = &activeQuad.syphonPosY;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("syphon scale X"))->value = &activeQuad.syphonScaleX;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("syphon scale Y"))->value = &activeQuad.syphonScaleY;

    #endif
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("image on/off"))->value = &activeQuad.imgBg;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("img scale X"))->value = &activeQuad.imgMultX;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("img scale Y"))->value = &activeQuad.imgMultY;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("H mirror"))->value = &activeQuad.imgHFlip;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("V mirror"))->value = &activeQuad.imgVFlip;
    dynamic_cast<ofxSimpleGuiColorPicker*>(firstPage.findControlByName("img color"))->value = &activeQuad.imgColorize.r;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("blending on/off"))->value = &activeQuad.bBlendModes;
    dynamic_cast<ofxSimpleGuiComboBox*>(firstPage.findControlByName("blending mode"))->m_selectedChoice = &activeQuad.blendMode;

    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("solid bg on/off"))->value = &activeQuad.colorBg;
    dynamic_cast<ofxSimpleGuiColorPicker*>(firstPage.findControlByName("Color"))->value = &activeQuad.bgColor.r;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("transition color"))->value = &activeQuad.transBg;
    dynamic_cast<ofxSimpleGuiColorPicker*>(firstPage.findControlByName("second Color"))->value = &activeQuad.secondColor.r;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("trans duration"))->value = &activeQuad.transDuration;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("mask on/off"))->value = &activeQuad.bMask;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("invert mask"))->value = &activeQuad.maskInvert;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("surface deform."))->value = &activeQuad.bDeform;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("use bezier"))->value = &activeQuad.bBezier;
    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("use grid"))->value = &activeQuad.bGrid;
    dynamic_cast<ofxSimpleGuiSliderInt*>(firstPage.findControlByName("grid rows"))->value = &activeQuad.gridRows;
    dynamic_cast<ofxSimpleGuiSliderInt*>(firstPage.findControlByName("grid columns"))->value = &activeQuad.gridColumns;

    dynamic_cast<ofxSimpleGuiToggle*>(firstPage.findControlByName("edge blend on/off"))->value = &activeQuad.edgeBlendBool;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("power"))->value = &activeQuad.edgeBlendExponent;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("gamma"))->value = &activeQuad.edgeBlendGamma;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("luminance"))->value = &activeQuad.edgeBlendLuminance;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("left edge"))->value = &activeQuad.edgeBlendAmountSin;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("right edge"))->value = &activeQuad.edgeBlendAmountDx;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("top edge"))->value = &activeQuad.edgeBlendAmountTop;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(firstPage.findControlByName("bottom edge"))->value = &activeQuad.edgeBlendAmountBottom;
    dynamic_cast<ofxSimpleGuiSliderInt*>(firstPage.findControlByName("X displacement"))->value = &activeQuad.quadDispX;
    dynamic_cast<ofxSimpleGuiSliderInt*>(firstPage.findControlByName("Y displacement"))->value = &activeQuad.quadDispY;
    dynamic_cast<ofxSimpleGuiSliderInt*>(firstPage.findControlByName("Width"))->value = &activeQuad.quadW;
    dynamic_cast<ofxSimpleGuiSliderInt*>(firstPage.findControlByName("Height"))->value = &activeQuad.quadH;

    // Second Page
    ofxSimpleGuiPage& secondPage = gui.page("Page 2");
    dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("video on/off"))->value = &activeQuad.videoBg;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("video scale X"))->value = &activeQuad.videoMultX;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("video scale Y"))->value = &activeQuad.videoMultY;
    dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("H mirror"))->value = &activeQuad.videoHFlip;
    dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("V mirror"))->value = &activeQuad.videoVFlip;
    dynamic_cast<ofxSimpleGuiColorPicker*>(secondPage.findControlByName("video color"))->value = &activeQuad.videoColorize.r;
    dynamic_cast<ofxSimpleGuiSliderInt*>(secondPage.findControlByName("video sound vol"))->value = &activeQuad.videoVolume;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("video speed"))->value = &activeQuad.videoSpeed;
    dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("video loop"))->value = &activeQuad.videoLoop;
    dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("video greenscreen"))->value = &activeQuad.videoGreenscreen;
    dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("shared video on/off"))->value = &activeQuad.sharedVideoBg;
    dynamic_cast<ofxSimpleGuiSliderInt*>(secondPage.findControlByName("shared video"))->value = &activeQuad.sharedVideoNum;

    if (m_app->m_cameras.size() > 0)
    {
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("cam on/off"))->value = &activeQuad.camBg;
        if(m_app->m_cameras.size() > 1)
        {
            dynamic_cast<ofxSimpleGuiComboBox*>(secondPage.findControlByName("select camera"))->m_selectedChoice = &activeQuad.camNumber;
        }
        dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("camera scale X"))->value = &activeQuad.camMultX;
        dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("camera scale Y"))->value = &activeQuad.camMultY;
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("flip H"))->value = &activeQuad.camHFlip;
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("flip V"))->value = &activeQuad.camVFlip;
        dynamic_cast<ofxSimpleGuiColorPicker*>(secondPage.findControlByName("cam color"))->value = &activeQuad.camColorize.r;
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("camera greenscreen"))->value = &activeQuad.camGreenscreen;
    }
    dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("g-screen threshold"))->value = &activeQuad.thresholdGreenscreen;
    dynamic_cast<ofxSimpleGuiColorPicker*>(secondPage.findControlByName("greenscreen col"))->value = &activeQuad.colorGreenscreen.r;
    dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("slideshow on/off"))->value = &activeQuad.slideshowBg;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("slide duration"))->value = &activeQuad.slideshowSpeed;
    dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("slides to quad size"))->value = &activeQuad.slideFit;
    dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("keep aspect ratio"))->value = &activeQuad.slideKeepAspect;

    #ifdef WITH_KINECT
    if(m_app->m_isKinectInitialized)
    {
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("use kinect"))->value = &activeQuad.kinectBg;
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("show kinect image"))->value = &activeQuad.kinectImg;
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("show kinect gray image"))->value = &activeQuad.getKinectGrayImage;
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("use kinect as mask"))->value = &activeQuad.kinectMask;
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("kinect blob detection"))->value = &activeQuad.getKinectContours;
        dynamic_cast<ofxSimpleGuiToggle*>(secondPage.findControlByName("blob curved contour"))->value = &activeQuad.kinectContourCurved;
        dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("kinect scale X"))->value = &activeQuad.kinectMultX;
        dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("kinect scale Y"))->value = &activeQuad.kinectMultY;
        dynamic_cast<ofxSimpleGuiColorPicker*>(secondPage.findControlByName("kinect color"))->value = &activeQuad.kinectColorize.r;
        dynamic_cast<ofxSimpleGuiSliderInt*>(secondPage.findControlByName("near threshold"))->value = &activeQuad.nearDepthTh;
        dynamic_cast<ofxSimpleGuiSliderInt*>(secondPage.findControlByName("far threshold"))->value = &activeQuad.farDepthTh;
        dynamic_cast<ofxSimpleGuiSliderInt*>(secondPage.findControlByName("kinect image blur"))->value = &activeQuad.kinectBlur;
        dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("blob min area"))->value = &activeQuad.kinectContourMin;
        dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("blob max area"))->value = &activeQuad.kinectContourMax;
        dynamic_cast<ofxSimpleGuiSliderInt*>(secondPage.findControlByName("blob contour smooth"))->value = &activeQuad.kinectContourSmooth;
        dynamic_cast<ofxSimpleGuiSliderFloat*>(secondPage.findControlByName("blob simplify"))->value = &activeQuad.kinectContourSimplify;
    }
    #endif

    ofxSimpleGuiPage& thirdPage = gui.page("Page 3");
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("corner 0 X"))->value = &activeQuad.corners[0].x;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("corner 0 Y"))->value = &activeQuad.corners[0].y;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("corner 3 X"))->value = &activeQuad.corners[3].x;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("corner 3 Y"))->value = &activeQuad.corners[3].y;

    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("corner 1 X"))->value = &activeQuad.corners[1].x;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("corner 1 Y"))->value = &activeQuad.corners[1].y;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("corner 2 X"))->value = &activeQuad.corners[2].x;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("corner 2 Y"))->value = &activeQuad.corners[2].y;

    dynamic_cast<ofxSimpleGuiToggle*>(thirdPage.findControlByName("mask on/off"))->value = &activeQuad.bMask;
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("top"))->value = &activeQuad.crop[0];
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("right"))->value = &activeQuad.crop[1];
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("bottom"))->value = &activeQuad.crop[2];
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("left"))->value = &activeQuad.crop[3];
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("center X"))->value = &activeQuad.circularCrop[0];
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("center Y"))->value = &activeQuad.circularCrop[1];
    dynamic_cast<ofxSimpleGuiSliderFloat*>(thirdPage.findControlByName("radius"))->value = &activeQuad.circularCrop[2];
}
