#include "testApp.h"

//--------------------------------------------------------------

void testApp::setXml()

{
    XML.setValue("GENERAL:ACTIVE_QUAD",activeQuad);
    XML.setValue("GENERAL:N_OF_QUADS",nOfQuads);

    for(int i = 0; i < 36; i++)
    {
        if (quads[i].initialized)
        {

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":NUMBER",quads[i].quadNumber);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":LAYER",quads[i].layer);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CONTENT:DISPX",quads[i].quadDispX);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CONTENT:DISPY",quads[i].quadDispY);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CONTENT:WIDTH",quads[i].quadW);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CONTENT:HEIGHT",quads[i].quadH);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:LOADED_IMG",quads[i].loadedImg);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:LOADED_IMG_PATH",quads[i].bgImg);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:LOADED_VIDEO",quads[i].loadedVideo);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:LOADED_VIDEO_PATH",quads[i].bgVideo);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:LOADED_SLIDESHOW",quads[i].bgSlideshow);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_0:X",quads[i].corners[0].x);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_0:Y",quads[i].corners[0].y);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_1:X",quads[i].corners[1].x);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_1:Y",quads[i].corners[1].y);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_2:X",quads[i].corners[2].x);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_2:Y",quads[i].corners[2].y);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_3:X",quads[i].corners[3].x);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_3:Y",quads[i].corners[3].y);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IS_ON",quads[i].isOn);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:ACTIVE",quads[i].colorBg);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:TRANS:ACTIVE",quads[i].transBg);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:TRANS:DURATION",quads[i].transDuration);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:ACTIVE",quads[i].slideshowBg);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:SPEED",quads[i].slideshowSpeed);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:FIT",quads[i].slideFit);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:KEEP_ASPECT",quads[i].slideKeepAspect);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:ACTIVE",quads[i].camBg);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:ACTIVE",quads[i].imgBg);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:ACTIVE",quads[i].videoBg);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:WIDTH",quads[i].camWidth);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:HEIGHT",quads[i].camHeight);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:MULT_X",quads[i].camMultX);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:MULT_Y",quads[i].camMultY);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:MULT_X",quads[i].imgMultX);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:MULT_Y",quads[i].imgMultY);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:MULT_X",quads[i].videoMultX);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:MULT_Y",quads[i].videoMultY);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:SPEED",quads[i].videoSpeed);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:VOLUME",quads[i].videoVolume);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:LOOP",quads[i].videoLoop);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:R",quads[i].bgColor.r);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:G",quads[i].bgColor.g);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:B",quads[i].bgColor.b);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:A",quads[i].bgColor.a);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:SECOND_COLOR:R",quads[i].secondColor.r);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:SECOND_COLOR:G",quads[i].secondColor.g);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:SECOND_COLOR:B",quads[i].secondColor.b);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":COLOR:SECOND_COLOR:A",quads[i].secondColor.a);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:COLORIZE:R",quads[i].imgColorize.r);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:COLORIZE:G",quads[i].imgColorize.g);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:COLORIZE:B",quads[i].imgColorize.b);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:COLORIZE:A",quads[i].imgColorize.a);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:COLORIZE:R",quads[i].videoColorize.r);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:COLORIZE:G",quads[i].videoColorize.g);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:COLORIZE:B",quads[i].videoColorize.b);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:COLORIZE:A",quads[i].videoColorize.a);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:COLORIZE:R",quads[i].camColorize.r);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:COLORIZE:G",quads[i].camColorize.g);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:COLORIZE:B",quads[i].camColorize.b);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:COLORIZE:A",quads[i].camColorize.a);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:FLIP:H",quads[i].imgHFlip);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":IMG:FLIP:V",quads[i].imgVFlip);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:FLIP:H",quads[i].videoHFlip);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:FLIP:V",quads[i].videoVFlip);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:FLIP:H",quads[i].camHFlip);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":CAM:FLIP:V",quads[i].camVFlip);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":BLENDING:ON",quads[i].bBlendModes);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":BLENDING:MODE",quads[i].blendMode);

            XML.setValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:ON",quads[i].edgeBlendBool);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:EXPONENT",quads[i].edgeBlendExponent);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:GAMMA",quads[i].edgeBlendGamma);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:AMOUNT:SIN",quads[i].edgeBlendAmountSin);
            XML.setValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:AMOUNT:DX",quads[i].edgeBlendAmountDx);
        }
    }
}


void testApp::getXml()

{

    nOfQuads = XML.getValue("GENERAL:N_OF_QUADS", 0);
    activeQuad = XML.getValue("GENERAL:ACTIVE_QUAD", 0);

    for(int i = 0; i < nOfQuads; i++)
    {
        float x0 = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_0:X",0.0);
        float y0 = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_0:Y",0.0);
        float x1 = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_1:X",0.0);
        float y1 = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_1:Y",0.0);
        float x2 = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_2:X",0.0);
        float y2 = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_2:Y",0.0);
        float x3 = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_3:X",0.0);
        float y3 = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CORNERS:CORNER_3:Y",0.0);

        quads[i].setup(x0, y0, x1, y1, x2, y2, x3, y3, slideshowFolders, edgeBlendShader, quadMaskShader, camGrabber, kinect);
        quads[i].quadNumber = XML.getValue("QUADS:QUAD_"+ofToString(i)+":NUMBER", 0);
        quads[i].layer = XML.getValue("QUADS:QUAD_"+ofToString(i)+":LAYER", 0);
        layers[quads[i].layer] = quads[i].quadNumber;

        quads[i].quadDispX = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CONTENT:DISPX",0);
        quads[i].quadDispY = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CONTENT:DISPY",0);
        quads[i].quadW = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CONTENT:WIDTH",0);
        quads[i].quadH = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CONTENT:HEIGHT",0);

        quads[i].imgBg = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:ACTIVE",0);
        quads[i].loadedImg = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:LOADED_IMG", "", 0);
        quads[i].bgImg = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:LOADED_IMG_PATH", "", 0);
        if ((quads[i].imgBg) && (quads[i].bgImg != ""))
        {
            quads[i].loadImageFromFile(quads[i].loadedImg, quads[i].bgImg);
        }
        quads[i].imgHFlip = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:FLIP:H", 0);
        quads[i].imgVFlip = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:FLIP:V", 0);

        quads[i].videoBg = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:ACTIVE",0);
        quads[i].loadedVideo = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:LOADED_VIDEO", "", 0);
        quads[i].bgVideo = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:LOADED_VIDEO_PATH", "", 0);
        if ((quads[i].videoBg) && (quads[i].bgVideo != ""))
        {
            quads[i].loadVideoFromFile(quads[i].loadedVideo, quads[i].bgVideo);
        }
        quads[i].videoHFlip = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:FLIP:H", 0);
        quads[i].videoVFlip = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:FLIP:V", 0);

        quads[i].bgSlideshow = XML.getValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:LOADED_SLIDESHOW", 0);

        quads[i].colorBg = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:ACTIVE",0);

        quads[i].transBg = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:TRANS:ACTIVE",0);
        quads[i].transDuration = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:TRANS:DURATION", 1.0);
        quads[i].slideshowBg = XML.getValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:ACTIVE", 0);
        quads[i].slideshowSpeed = XML.getValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:SPEED", 1.0);
        quads[i].slideFit = XML.getValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:FIT", 0);
        quads[i].slideKeepAspect = XML.getValue("QUADS:QUAD_"+ofToString(i)+":SLIDESHOW:KEEP_ASPECT", 1);

        quads[i].camBg = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:ACTIVE",0);
        quads[i].camWidth = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:WIDTH",0);
        quads[i].camHeight = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:HEIGHT",0);
        quads[i].camHFlip = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:FLIP:H", 0);
        quads[i].camVFlip = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:FLIP:V", 0);

        quads[i].camMultX = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:MULT_X",1.0);
        quads[i].camMultY = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:MULT_Y",1.0);
        quads[i].imgMultX = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:MULT_X",1.0);
        quads[i].imgMultY = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:MULT_Y",1.0);
        quads[i].videoMultX = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:MULT_X",1.0);
        quads[i].videoMultY = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:MULT_Y",1.0);
        quads[i].videoSpeed = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:SPEED",1.0);
        quads[i].videoVolume = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:VOLUME",0);
        quads[i].videoLoop = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:LOOP",1);

        quads[i].bgColor.r = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:R",0.0);
        quads[i].bgColor.g = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:G",0.0);
        quads[i].bgColor.b = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:B",0.0);
        quads[i].bgColor.a = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:A",0.0);

        quads[i].secondColor.r = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:SECOND_COLOR:R",0.0);
        quads[i].secondColor.g = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:SECOND_COLOR:G",0.0);
        quads[i].secondColor.b = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:SECOND_COLOR:B",0.0);
        quads[i].secondColor.a = XML.getValue("QUADS:QUAD_"+ofToString(i)+":COLOR:SECOND_COLOR:A",0.0);

        quads[i].imgColorize.r = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:COLORIZE:R",1.0);
        quads[i].imgColorize.g = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:COLORIZE:G",1.0);
        quads[i].imgColorize.b = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:COLORIZE:B",1.0);
        quads[i].imgColorize.a = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IMG:COLORIZE:A",1.0);

        quads[i].videoColorize.r = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:COLORIZE:R",1.0);
        quads[i].videoColorize.g = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:COLORIZE:G",1.0);
        quads[i].videoColorize.b = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:COLORIZE:B",1.0);
        quads[i].videoColorize.a = XML.getValue("QUADS:QUAD_"+ofToString(i)+":VIDEO:COLORIZE:A",1.0);

        quads[i].camColorize.r = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:COLORIZE:R",1.0);
        quads[i].camColorize.g = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:COLORIZE:G",1.0);
        quads[i].camColorize.b = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:COLORIZE:B",1.0);
        quads[i].camColorize.a = XML.getValue("QUADS:QUAD_"+ofToString(i)+":CAM:COLORIZE:A",1.0);

        quads[i].bBlendModes = XML.getValue("QUADS:QUAD_"+ofToString(i)+":BLENDING:ON", 0);
        quads[i].blendMode= XML.getValue("QUADS:QUAD_"+ofToString(i)+":BLENDING:MODE", 0);

        quads[i].edgeBlendBool = XML.getValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:ON", 0);
        quads[i].edgeBlendExponent = XML.getValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:EXPONENT", 1.0);
        quads[i].edgeBlendGamma = XML.getValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:GAMMA", 1.8);
        quads[i].edgeBlendAmountSin = XML.getValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:AMOUNT:SIN", 0.3);
        quads[i].edgeBlendAmountDx = XML.getValue("QUADS:QUAD_"+ofToString(i)+":EDGE_BLENDING:AMOUNT:DX", 0.3);

        quads[i].isOn = XML.getValue("QUADS:QUAD_"+ofToString(i)+":IS_ON",0);

    }
}