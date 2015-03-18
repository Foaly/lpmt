#include "testApp.h"

//--------------------------------------------------------------

void testApp::saveCurrentSettingsToXMLFile(std::string xmlFilePath)
{
    ofxXmlSettings xmlSettingsFile;

    // update the XML object with the current settings, before saving it
    xmlSettingsFile.setValue("GENERAL:ACTIVE_QUAD", m_activeQuad - m_quads.begin()); // the index of the active quad
    xmlSettingsFile.setValue("GENERAL:N_OF_QUADS", static_cast<int>(m_quads.size()));
    xmlSettingsFile.setValue("TIMELINE:USE_TIMELINE",useTimeline);
    xmlSettingsFile.setValue("TIMELINE:DURATION",timelineDurationSeconds);
    for(int j=0; j<4; j++)
    {
        xmlSettingsFile.setValue("SHARED_VIDEOS:VIDEO_"+ofToString(j)+":PATH", sharedVideosFiles[j]);
    }

    int i = 0;
    for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++, i++)
    {
        const std::string number = ofToString(i);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":NUMBER",quad->quadNumber);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":TIMELINE:TINT",quad->bTimelineTint);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":TIMELINE:COLOR",quad->bTimelineColor);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":TIMELINE:ALPHA",quad->bTimelineAlpha);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":TIMELINE:SLIDE",quad->bTimelineSlideChange);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CONTENT:DISPX",quad->quadDispX);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CONTENT:DISPY",quad->quadDispY);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CONTENT:WIDTH",quad->quadW);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CONTENT:HEIGHT",quad->quadH);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:LOADED_IMG",quad->loadedImg);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:LOADED_IMG_PATH",quad->bgImg);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:LOADED_VIDEO",quad->loadedVideo);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:LOADED_VIDEO_PATH",quad->bgVideo);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":SLIDESHOW:LOADED_SLIDESHOW",quad->bgSlideshow);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_0:X",quad->corners[0].x);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_0:Y",quad->corners[0].y);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_1:X",quad->corners[1].x);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_1:Y",quad->corners[1].y);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_2:X",quad->corners[2].x);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_2:Y",quad->corners[2].y);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_3:X",quad->corners[3].x);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_3:Y",quad->corners[3].y);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IS_ON",quad->isOn);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:ACTIVE",quad->colorBg);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:TRANS:ACTIVE",quad->transBg);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:TRANS:DURATION",quad->transDuration);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":SLIDESHOW:ACTIVE",quad->slideshowBg);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":SLIDESHOW:SPEED",quad->slideshowSpeed);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":SLIDESHOW:FIT",quad->slideFit);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":SLIDESHOW:KEEP_ASPECT",quad->slideKeepAspect);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:ACTIVE",quad->camBg);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:ACTIVE",quad->imgBg);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:ACTIVE",quad->videoBg);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:WIDTH",quad->camWidth);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:HEIGHT",quad->camHeight);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:MULT_X",quad->camMultX);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:MULT_Y",quad->camMultY);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:MULT_X",quad->imgMultX);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:MULT_Y",quad->imgMultY);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:MULT_X",quad->videoMultX);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:MULT_Y",quad->videoMultY);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:SPEED",quad->videoSpeed);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:VOLUME",quad->videoVolume);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:LOOP",quad->videoLoop);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:R",quad->bgColor.r);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:G",quad->bgColor.g);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:B",quad->bgColor.b);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:A",quad->bgColor.a);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:SECOND_COLOR:R",quad->secondColor.r);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:SECOND_COLOR:G",quad->secondColor.g);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:SECOND_COLOR:B",quad->secondColor.b);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":COLOR:SECOND_COLOR:A",quad->secondColor.a);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:COLORIZE:R",quad->imgColorize.r);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:COLORIZE:G",quad->imgColorize.g);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:COLORIZE:B",quad->imgColorize.b);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:COLORIZE:A",quad->imgColorize.a);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:COLORIZE:R",quad->videoColorize.r);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:COLORIZE:G",quad->videoColorize.g);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:COLORIZE:B",quad->videoColorize.b);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:COLORIZE:A",quad->videoColorize.a);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:COLORIZE:R",quad->camColorize.r);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:COLORIZE:G",quad->camColorize.g);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:COLORIZE:B",quad->camColorize.b);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:COLORIZE:A",quad->camColorize.a);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:FLIP:H",quad->imgHFlip);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":IMG:FLIP:V",quad->imgVFlip);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:FLIP:H",quad->videoHFlip);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":VIDEO:FLIP:V",quad->videoVFlip);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:FLIP:H",quad->camHFlip);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":CAM:FLIP:V",quad->camVFlip);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":SHARED_VIDEO:ACTIVE",quad->sharedVideoBg);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":SHARED_VIDEO:NUM",quad->sharedVideoNum);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":BLENDING:ON",quad->bBlendModes);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":BLENDING:MODE",quad->blendMode);

        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:ON",quad->edgeBlendBool);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:EXPONENT",quad->edgeBlendExponent);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:GAMMA",quad->edgeBlendGamma);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:LUMINANCE", quad->edgeBlendLuminance);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:AMOUNT:SIN",quad->edgeBlendAmountSin);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:AMOUNT:DX",quad->edgeBlendAmountDx);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:AMOUNT:TOP",quad->edgeBlendAmountTop);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:AMOUNT:BOTTOM",quad->edgeBlendAmountBottom);

        //mask stuff
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:ON",quad->bMask);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:INVERT_MASK",quad->maskInvert);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:N_POINTS",(int)quad->m_maskPoints.size());
        if (quad->m_maskPoints.size() > 0)
        {
            for(size_t j = 0; j < quad->m_maskPoints.size(); j++)
            {
                const std::string jString = ofToString(j);
                xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:POINTS:POINT_" + jString + ":X", quad->m_maskPoints[j].x);
                xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:POINTS:POINT_" + jString + ":Y", quad->m_maskPoints[j].y);
            }
        }
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:CROP:RECTANGULAR:TOP",quad->crop[0]);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:CROP:RECTANGULAR:RIGHT",quad->crop[1]);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:CROP:RECTANGULAR:BOTTOM",quad->crop[2]);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:CROP:RECTANGULAR:LEFT",quad->crop[3]);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:CROP:CIRCULAR:X",quad->circularCrop[0]);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:CROP:CIRCULAR:Y",quad->circularCrop[1]);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":MASK:CROP:CIRCULAR:RADIUS",quad->circularCrop[2]);
        // deform stuff
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":DEFORM:ON",quad->bDeform);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":DEFORM:BEZIER:ON",quad->bBezier);
        xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":DEFORM:GRID:ON",quad->bGrid);
        // bezier stuff
        for (int j = 0; j < 4; ++j)
        {
            const std::string jString = ofToString(j);
            for (int k = 0; k < 4; ++k)
            {
                const std::string kString = ofToString(k);
                xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":DEFORM:BEZIER:CTRLPOINTS:POINT_" + jString + "_" + kString + ":X", quad->bezierPoints[j][k][0]);
                xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":DEFORM:BEZIER:CTRLPOINTS:POINT_" + jString + "_" + kString + ":Y", quad->bezierPoints[j][k][1]);
                xmlSettingsFile.setValue("QUADS:QUAD_" + number + ":DEFORM:BEZIER:CTRLPOINTS:POINT_" + jString + "_" + kString + ":Z", quad->bezierPoints[j][k][2]);
            }
        }
    }

    // actually save the *.xml settings file
    const bool wasSavedSuccessful = xmlSettingsFile.saveFile(xmlFilePath);

    // error handeling
    if (wasSavedSuccessful)
    {
        std::cout << "Saved settings file to: \"" << xmlFilePath << "\"" << std::endl;
    }
    else
    {
        std::cout << "Error saving the settings file: \"" << xmlFilePath << "\"" << std::endl;
    }
}


void testApp::loadSettingsFromXMLFile(std::string xmlFilePath)
{
    ofxXmlSettings xmlSettingsFile;
    const bool wasLoadSuccessful = xmlSettingsFile.loadFile(xmlFilePath);

    if(wasLoadSuccessful)
    {
        std::cout << "Loaded settings file: \"" << xmlFilePath << "\"" << std::endl;

        const int numberOfQuads = xmlSettingsFile.getValue("GENERAL:N_OF_QUADS", 0);
        m_quads.clear();
        m_quads.reserve(numberOfQuads + 10);

        useTimeline = xmlSettingsFile.getValue("TIMELINE:USE_TIMELINE",0);
        timelineDurationSeconds = xmlSettingsFile.getValue("TIMELINE:DURATION",100);

        for(int j=0; j<4; j++)
        {
            string sharedVideoPath = xmlSettingsFile.getValue("SHARED_VIDEOS:VIDEO_"+ofToString(j)+":PATH", "");
            sharedVideosFiles[j] = sharedVideoPath;
            if(sharedVideoPath != "")
            {
                openSharedVideoFile(sharedVideoPath, j);
            }
        }

        for(int i = 0; i < numberOfQuads; i++)
        {
            Quad quad;

            const std::string number = ofToString(i);

            float x0 = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_0:X",0.0);
            float y0 = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_0:Y",0.0);
            float x1 = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_1:X",0.0);
            float y1 = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_1:Y",0.0);
            float x2 = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_2:X",0.0);
            float y2 = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_2:Y",0.0);
            float x3 = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_3:X",0.0);
            float y3 = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CORNERS:CORNER_3:Y",0.0);

            #ifdef WITH_KINECT
                #ifdef WITH_SYPHON
                quad.setup(ofPoint(x0, y0), ofPoint(x1, y1), ofPoint(x2, y2), ofPoint(x3, y3), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, syphClient, ttf);
                #else
                quad.setup(ofPoint(x0, y0), ofPoint(x1, y1), ofPoint(x2, y2), ofPoint(x3, y3), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, kinect, ttf);
                #endif
            #else
                #ifdef WITH_SYPHON
                quad.setup(ofPoint(x0, y0), ofPoint(x1, y1), ofPoint(x2, y2), ofPoint(x3, y3), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, syphClient, ttf);
                #else
                quad.setup(ofPoint(x0, y0), ofPoint(x1, y1), ofPoint(x2, y2), ofPoint(x3, y3), edgeBlendShader, quadMaskShader, chromaShader, m_cameras, sharedVideos, ttf);
                #endif
            #endif
            quad.quadNumber = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":NUMBER", 0);

            quad.bTimelineTint = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":TIMELINE:TINT",0);
            quad.bTimelineColor = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":TIMELINE:COLOR",0);
            quad.bTimelineAlpha = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":TIMELINE:ALPHA",0);
            quad.bTimelineSlideChange = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":TIMELINE:SLIDE",0);

            quad.quadDispX = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CONTENT:DISPX",0);
            quad.quadDispY = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CONTENT:DISPY",0);
            quad.quadW = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CONTENT:WIDTH",0);
            quad.quadH = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CONTENT:HEIGHT",0);

            quad.imgBg = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:ACTIVE",0);
            quad.loadedImg = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:LOADED_IMG", "", 0);
            quad.bgImg = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:LOADED_IMG_PATH", "", 0);
            if ((quad.imgBg) && (quad.bgImg != ""))
            {
                quad.loadImageFromFile(quad.loadedImg, quad.bgImg);
            }
            quad.imgHFlip = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:FLIP:H", 0);
            quad.imgVFlip = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:FLIP:V", 0);

            quad.videoBg = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:ACTIVE",0);
            quad.loadedVideo = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:LOADED_VIDEO", "", 0);
            quad.bgVideo = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:LOADED_VIDEO_PATH", "", 0);
            if ((quad.videoBg) && (quad.bgVideo != ""))
            {
                quad.loadVideoFromFile(quad.loadedVideo, quad.bgVideo);
            }
            quad.videoHFlip = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:FLIP:H", 0);
            quad.videoVFlip = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:FLIP:V", 0);

            quad.sharedVideoBg = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":SHARED_VIDEO:ACTIVE",0);
            quad.sharedVideoNum = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":SHARED_VIDEO:NUM", 1);

            quad.bgSlideshow = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":SLIDESHOW:LOADED_SLIDESHOW", 0);

            quad.colorBg = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:ACTIVE",0);

            quad.transBg = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:TRANS:ACTIVE",0);
            quad.transDuration = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:TRANS:DURATION", 1.0);
            quad.slideshowBg = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":SLIDESHOW:ACTIVE", 0);
            quad.slideshowSpeed = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":SLIDESHOW:SPEED", 1.0);
            quad.slideFit = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":SLIDESHOW:FIT", 0);
            quad.slideKeepAspect = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":SLIDESHOW:KEEP_ASPECT", 1);

            quad.camBg = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:ACTIVE",0);
            quad.camWidth = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:WIDTH",0);
            quad.camHeight = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:HEIGHT",0);
            quad.camHFlip = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:FLIP:H", 0);
            quad.camVFlip = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:FLIP:V", 0);

            quad.camMultX = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:MULT_X",1.0);
            quad.camMultY = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:MULT_Y",1.0);
            quad.imgMultX = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:MULT_X",1.0);
            quad.imgMultY = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:MULT_Y",1.0);
            quad.videoMultX = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:MULT_X",1.0);
            quad.videoMultY = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:MULT_Y",1.0);
            quad.videoSpeed = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:SPEED",1.0);
            quad.videoVolume = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:VOLUME",0);
            quad.videoLoop = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:LOOP",1);

            quad.bgColor.r = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:R",0.0);
            quad.bgColor.g = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:G",0.0);
            quad.bgColor.b = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:B",0.0);
            quad.bgColor.a = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:A",0.0);

            quad.secondColor.r = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:SECOND_COLOR:R",0.0);
            quad.secondColor.g = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:SECOND_COLOR:G",0.0);
            quad.secondColor.b = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:SECOND_COLOR:B",0.0);
            quad.secondColor.a = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":COLOR:SECOND_COLOR:A",0.0);

            quad.imgColorize.r = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:COLORIZE:R",1.0);
            quad.imgColorize.g = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:COLORIZE:G",1.0);
            quad.imgColorize.b = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:COLORIZE:B",1.0);
            quad.imgColorize.a = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IMG:COLORIZE:A",1.0);

            quad.videoColorize.r = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:COLORIZE:R",1.0);
            quad.videoColorize.g = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:COLORIZE:G",1.0);
            quad.videoColorize.b = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:COLORIZE:B",1.0);
            quad.videoColorize.a = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":VIDEO:COLORIZE:A",1.0);

            quad.camColorize.r = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:COLORIZE:R",1.0);
            quad.camColorize.g = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:COLORIZE:G",1.0);
            quad.camColorize.b = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:COLORIZE:B",1.0);
            quad.camColorize.a = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":CAM:COLORIZE:A",1.0);

            quad.bBlendModes = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":BLENDING:ON", 0);
            quad.blendMode= xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":BLENDING:MODE", 0);

            quad.edgeBlendBool = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:ON", 0);
            quad.edgeBlendExponent = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:EXPONENT", 1.0);
            quad.edgeBlendGamma = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:GAMMA", 1.8);
            quad.edgeBlendLuminance = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:LUMINANCE", 0.0);
            quad.edgeBlendAmountSin = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:AMOUNT:SIN", 0.3);
            quad.edgeBlendAmountDx = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:AMOUNT:DX", 0.3);
            quad.edgeBlendAmountTop = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:AMOUNT:TOP", 0.0);
            quad.edgeBlendAmountBottom = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":EDGE_BLENDING:AMOUNT:BOTTOM", 0.0);

            //mask stuff
            quad.bMask = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:ON", 0);
            quad.maskInvert = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:INVERT_MASK", 0);
            const int numberOfMaskPoints =  xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:N_POINTS", 0);
            quad.m_maskPoints.clear();
            if (numberOfMaskPoints > 0)
            {
                for(size_t j = 0; j < numberOfMaskPoints; j++)
                {
                    ofPoint tempMaskPoint;
                    tempMaskPoint.x = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:POINTS:POINT_"+ofToString(j)+":X", 0);
                    tempMaskPoint.y = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:POINTS:POINT_"+ofToString(j)+":Y", 0);
                    quad.m_maskPoints.push_back(tempMaskPoint);
                }
            }
            quad.crop[0] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:CROP:RECTANGULAR:TOP",0.0);
            quad.crop[1] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:CROP:RECTANGULAR:RIGHT",0.0);
            quad.crop[2] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:CROP:RECTANGULAR:BOTTOM",0.0);
            quad.crop[3] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:CROP:RECTANGULAR:LEFT",0.0);
            quad.circularCrop[0] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:CROP:CIRCULAR:X",0.5);
            quad.circularCrop[1] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:CROP:CIRCULAR:Y",0.5);
            quad.circularCrop[2] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":MASK:CROP:CIRCULAR:RADIUS",0.0);
            // deform stuff
            quad.bDeform = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":DEFORM:ON",0);
            quad.bBezier = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":DEFORM:BEZIER:ON",0);
            quad.bGrid = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":DEFORM:GRID:ON",0);
            // bezier stuff
            for (int j = 0; j < 4; ++j)
            {
                for (int k = 0; k < 4; ++k)
                {
                    quad.bezierPoints[j][k][0] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":DEFORM:BEZIER:CTRLPOINTS:POINT_"+ofToString(j)+"_"+ofToString(k)+":X",0.0);
                    quad.bezierPoints[j][k][1] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":DEFORM:BEZIER:CTRLPOINTS:POINT_"+ofToString(j)+"_"+ofToString(k)+":Y",0.0);
                    quad.bezierPoints[j][k][2] = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":DEFORM:BEZIER:CTRLPOINTS:POINT_"+ofToString(j)+"_"+ofToString(k)+":Z",0.0);
                }
            }


            quad.isOn = xmlSettingsFile.getValue("QUADS:QUAD_" + number + ":IS_ON",0);
            quad.isActive = false;

            m_quads.push_back(quad);
        }

        int activeQuadIndex = xmlSettingsFile.getValue("GENERAL:ACTIVE_QUAD", 0);
        if(activeQuadIndex < 0)
            activeQuadIndex = 0;
        else if (activeQuadIndex >= numberOfQuads)
            activeQuadIndex = numberOfQuads - 1;

        m_activeQuad = m_quads.begin() + activeQuadIndex;

        m_activeQuad->isActive = true;
        m_gui.updatePages(*m_activeQuad);
        m_gui.showPage(2);

        glDisable(GL_DEPTH_TEST);
    }
    else
    {
        // load was not succesful, maybe the xml is malformatted
        std::cout << "Error loading the settings file: \"" << xmlFilePath << "\" (make sure it is a properly formatted *.xml file)" << std::endl;
    }
}
