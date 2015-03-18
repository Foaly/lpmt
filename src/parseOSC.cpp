#include "testApp.h"

//--------------------------------------------------------------
void testApp::parseOsc()
{
    // get the next message
    ofxOscMessage m;
    receiver.getNextMessage( &m );
    std::string messageAddress = m.getAddress();

    // check for quads corner x movements
    if ( messageAddress == "/corners/x" )
    {
        // arguments are iif
        int osc_quad = m.getArgAsInt32( 0 );
        int osc_corner = m.getArgAsInt32( 1 );
        float osc_coord = m.getArgAsFloat( 2 );

        if(osc_quad >= m_quads.size())
        {
            osc_quad = m_quads.size() - 1;
        }
        else if(osc_quad < 0)
        {
            osc_quad = 0;
        }

        m_quads[osc_quad].corners[osc_corner].x = osc_coord;
    }
    // check for quads corner y movements
    else if ( messageAddress == "/corners/y" )
    {
        // arguments are iif
        int osc_quad = m.getArgAsInt32( 0 );
        int osc_corner = m.getArgAsInt32( 1 );
        float osc_coord = m.getArgAsFloat( 2 );

        if(osc_quad >= m_quads.size())
        {
            osc_quad = m_quads.size() - 1;
        }
        else if(osc_quad < 0)
        {
            osc_quad = 0;
        }

        m_quads[osc_quad].corners[osc_corner].y = osc_coord;
    }

    // check for active quad corner x movements
    if ( messageAddress == "/active/corners/0" )
    {
        // arguments are ff
        float osc_coord_x = m.getArgAsFloat( 0 );
        float osc_coord_y = m.getArgAsFloat( 1 );
        m_activeQuad->corners[0].x = osc_coord_x;
        m_activeQuad->corners[0].y = osc_coord_y;
    }

    if ( messageAddress == "/active/corners/1" )
    {
        // arguments are ff
        float osc_coord_x = m.getArgAsFloat( 0 );
        float osc_coord_y = m.getArgAsFloat( 1 );
        m_activeQuad->corners[1].x = osc_coord_x;
        m_activeQuad->corners[1].y = osc_coord_y;
    }

    if ( messageAddress == "/active/corners/2" )
    {
        // arguments are ff
        float osc_coord_x = m.getArgAsFloat( 0 );
        float osc_coord_y = m.getArgAsFloat( 1 );
        m_activeQuad->corners[2].x = osc_coord_x;
        m_activeQuad->corners[2].y = osc_coord_y;
    }

    if ( messageAddress == "/active/corners/3" )
    {
        // arguments are ff
        float osc_coord_x = m.getArgAsFloat( 0 );
        float osc_coord_y = m.getArgAsFloat( 1 );
        m_activeQuad->corners[3].x = osc_coord_x;
        m_activeQuad->corners[3].y = osc_coord_y;
    }


    // resync
    else if ( messageAddress == "/projection/resync" )
    {
        resync();
    }

    // stop
    else if ( messageAddress == "/projection/stop" )
    {
        stopProjection();
    }

    // start
    else if ( messageAddress == "/projection/start" )
    {
        startProjection();
    }

    // save
    else if ( messageAddress == "/projection/save" )
    {
        ofFileDialogResult dialog_result = ofSystemSaveDialog("lpmt_settings.xml", "Save settings file (.xml)");

        if(dialog_result.bSuccess)
        {
            saveCurrentSettingsToXMLFile(dialog_result.getPath());
        }
    }

    // load
    else if ( messageAddress == "/projection/load" )
    {
        loadSettingsFromXMLFile("_lpmt_settings.xml");
        m_gui.updatePages(*m_activeQuad);
        m_gui.showPage(2);
    }

    // toggle fullscreen
    else if ( messageAddress == "/projection/fullscreen/toggle" )
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

    else if ( messageAddress == "/projection/fullscreen/on" )
    {
        bFullscreen = true;
        ofSetFullscreen(true);
    }

    else if ( messageAddress == "/projection/fullscreen/off" )
    {
        bFullscreen = false;
        ofSetWindowShape(WINDOW_W, WINDOW_H);
        ofSetFullscreen(false);
        // figure out how to put the window in the center:
        int screenW = ofGetScreenWidth();
        int screenH = ofGetScreenHeight();
        ofSetWindowPosition(screenW/2-WINDOW_W/2, screenH/2-WINDOW_H/2);
    }

    // toggle gui
    else if ( messageAddress == "/projection/gui/toggle" )
    {
        m_gui.toggleDraw();
        bGui = !bGui;
    }

    else if ( messageAddress == "/projection/mode/masksetup/toggle" )
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

    else if ( messageAddress == "/projection/mode/masksetup/on" )
    {
        if (!bGui)
        {
            maskSetup = true;
            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                quad->isMaskSetup = true;
            }
        }
    }

    else if ( messageAddress == "/projection/mode/masksetup/off" )
    {
        if (!bGui)
        {
            maskSetup = false;
            for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
            {
                quad->isMaskSetup = false;
            }
        }
    }

    // toggle setup
    else if ( messageAddress == "/projection/mode/setup/toggle" )
    {
        isSetup = !isSetup;
        for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
        {
            quad->isSetup = !quad->isSetup;
        }
    }

    else if ( messageAddress == "/projection/mode/setup/on" )
    {
        isSetup = true;
        for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
        {
            quad->isSetup = true;
        }
    }

    else if ( messageAddress == "/projection/mode/setup/off" )
    {
        isSetup = false;
        for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
        {
            quad->isSetup = false;
        }
    }

    // connects to mpe server
    else if ( messageAddress == "/projection/mpe/connect" )
    {
        mpeSetup();
    }


    // timeline stuff
#ifdef WITH_TIMELINE
    // use toggle
    else if ( messageAddress == "/projection/timeline/toggle" )
    {
        // no argument
        useTimeline = !useTimeline;
    }

    // use
    else if ( messageAddress == "/projection/timeline/use" )
    {
        // argument is int32
        int osc_timeline = m.getArgAsInt32( 0 );
        if(osc_timeline == 0)
        {
            useTimeline = false;
        }
        else if(osc_timeline == 1)
        {
            useTimeline = true;
        }
    }

    // timeline duration in seconds
    else if ( messageAddress == "/projection/timeline/duration" )
    {
        // argument is float
        float osc_timelineDurationSeconds = m.getArgAsFloat( 0 );
        if(osc_timelineDurationSeconds >= 10.0)
        {
            timelineDurationSeconds =osc_timelineDurationSeconds;
        }
    }

    else if ( messageAddress == "/projection/timeline/start")
    {
        timeline.togglePlay();
    }

    else if ( messageAddress == "/projection/timeline/show")
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
            timeline.disable();
        }
    }

#endif

//------------------------------------------------------
// active quad stuff
//------------------------------------------------------

    // change active quad
    else if ( messageAddress == "/active/set" )
    {
        // argument is int32
        int quadToBeActivated = m.getArgAsInt32( 0 );

        if(quadToBeActivated >= m_quads.size())
        {
            quadToBeActivated = m_quads.size() - 1;
        }
        else if(quadToBeActivated < 0)
        {
            quadToBeActivated = 0;
        }

        if (m_quads.begin() + quadToBeActivated != m_activeQuad)
        {
            m_activeQuad->isActive = false;
            m_activeQuad = m_quads.begin() + quadToBeActivated;
            m_activeQuad->isActive = true;
            m_gui.updatePages(*m_activeQuad);
        }
    }

    // on/off
    else if ( messageAddress == "/active/show" )
    {
        // argument is int32
        int osc_quad_isOn = m.getArgAsInt32( 0 );
        if(osc_quad_isOn == 0)
        {
            m_activeQuad->isOn = false;
        }
        else if(osc_quad_isOn == 1)
        {
            m_activeQuad->isOn = true;
        }
    }

    // use timeline color
    else if ( messageAddress == "/active/timeline/color" )
    {
        // argument is int32
        int osc_quad_bTimelineColor = m.getArgAsInt32( 0 );
        if(osc_quad_bTimelineColor == 0)
        {
            m_activeQuad->bTimelineColor = false;
        }
        else if(osc_quad_bTimelineColor == 1)
        {
            m_activeQuad->bTimelineColor = true;
        }
    }

    // use timeline color
    else if ( messageAddress == "/active/timeline/alpha" )
    {
        // argument is int32
        int osc_quad_bTimelineAlpha = m.getArgAsInt32( 0 );
        if(osc_quad_bTimelineAlpha == 0)
        {
            m_activeQuad->bTimelineAlpha = false;
        }
        else if(osc_quad_bTimelineAlpha == 1)
        {
            m_activeQuad->bTimelineAlpha = true;
        }
    }

    // use timeline for slides
    else if ( messageAddress == "/active/timeline/slides" )
    {
        // argument is int32
        int osc_quad_bTimelineSlideChange = m.getArgAsInt32( 0 );
        if(osc_quad_bTimelineSlideChange == 0)
        {
            m_activeQuad->bTimelineSlideChange = false;
        }
        else if(osc_quad_bTimelineSlideChange == 1)
        {
            m_activeQuad->bTimelineSlideChange = true;
        }
    }

    // img stuff on active quad
    else if ( messageAddress == "/active/img" )
    {
        m_activeQuad->imgBg = !m_activeQuad->imgBg;
    }

    // img on/off
    else if ( messageAddress == "/active/img/show" )
    {
        // argument is int32
        int osc_quad_imgBg = m.getArgAsInt32( 0 );
        if(osc_quad_imgBg == 0)
        {
            m_activeQuad->imgBg = false;
        }
        else if(osc_quad_imgBg == 1)
        {
            m_activeQuad->imgBg = true;
        }
    }

    // img load
    else if ( messageAddress == "/active/img/load" )
    {
        // no argument
        openImageFile();
    }

    // img HFlip
    else if ( messageAddress == "/active/img/hmirror" )
    {
        // argument is int32
        int osc_quad_imgHFlip = m.getArgAsInt32( 0 );
        if(osc_quad_imgHFlip == 0)
        {
            m_activeQuad->imgHFlip = false;
        }
        else if(osc_quad_imgHFlip == 1)
        {
            m_activeQuad->imgHFlip = true;
        }
    }

    // img VFlip
    else if ( messageAddress == "/active/img/vmirror" )
    {
        // argument is int32
        int osc_quad_imgVFlip = m.getArgAsInt32( 0 );
        if(osc_quad_imgVFlip == 0)
        {
            m_activeQuad->imgVFlip = false;
        }
        else if(osc_quad_imgVFlip == 1)
        {
            m_activeQuad->imgVFlip = true;
        }
    }

    else if ( messageAddress == "/active/img/color" )
    {
        // arguments are ffff
        float img_color_r = m.getArgAsFloat( 0 );
        float img_color_g = m.getArgAsFloat( 1 );
        float img_color_b = m.getArgAsFloat( 2 );
        float img_color_a = m.getArgAsFloat( 3 );
        m_activeQuad->imgColorize.r = img_color_r;
        m_activeQuad->imgColorize.g = img_color_g;
        m_activeQuad->imgColorize.b = img_color_b;
        m_activeQuad->imgColorize.a = img_color_a;
    }

    else if ( messageAddress == "/active/img/color/1" )
    {
        // arguments are f
        float img_color_r = m.getArgAsFloat( 0 );
        m_activeQuad->imgColorize.r = img_color_r;
    }

    else if ( messageAddress == "/active/img/color/2" )
    {
        // arguments are f
        float img_color_g = m.getArgAsFloat( 0 );
        m_activeQuad->imgColorize.g = img_color_g;
    }

    else if ( messageAddress == "/active/img/color/3" )
    {
        // arguments are f
        float img_color_b = m.getArgAsFloat( 0 );
        m_activeQuad->imgColorize.b = img_color_b;
    }

    else if ( messageAddress == "/active/img/color/4" )
    {
        // arguments are f
        float img_color_a = m.getArgAsFloat( 0 );
        m_activeQuad->imgColorize.a = img_color_a;
    }

    else if ( messageAddress == "/active/img/mult/x" )
    {
        // arguments are f
        float img_mult_x = m.getArgAsFloat( 0 );
        m_activeQuad->imgMultX = img_mult_x;
    }

    else if ( messageAddress == "/active/img/mult/y" )
    {
        // arguments are f
        float img_mult_y = m.getArgAsFloat( 0 );
        m_activeQuad->imgMultY = img_mult_y;
    }


    // blendModes on/off
    else if ( messageAddress == "/active/blendmodes/show" )
    {
        // argument is int32
        int osc_quad_bBlendModes = m.getArgAsInt32( 0 );
        if(osc_quad_bBlendModes == 0)
        {
            m_activeQuad->bBlendModes = false;
        }
        else if(osc_quad_bBlendModes == 1)
        {
            m_activeQuad->bBlendModes = true;
        }
    }

    // blendModes mode
    else if ( messageAddress == "/active/blendmodes/mode" )
    {
        // argument is int32
        int osc_quad_blendMode = m.getArgAsInt32( 0 );
        if(osc_quad_blendMode < 6)
        {
            m_activeQuad->blendMode = osc_quad_blendMode;
        }
    }

    // solid color stuff
    else if ( messageAddress == "/active/solid" )
    {
        m_activeQuad->colorBg = !m_activeQuad->colorBg;
    }

    // solid on/off
    else if ( messageAddress == "/active/solid/show" )
    {
        // argument is int32
        int osc_quad_colorBg = m.getArgAsInt32( 0 );
        if(osc_quad_colorBg == 0)
        {
            m_activeQuad->colorBg = false;
        }
        else if(osc_quad_colorBg == 1)
        {
            m_activeQuad->colorBg = true;
        }
    }

    else if ( messageAddress == "/active/solid/color" )
    {
        // arguments are ffff
        float solid_color_r = m.getArgAsFloat( 0 );
        float solid_color_g = m.getArgAsFloat( 1 );
        float solid_color_b = m.getArgAsFloat( 2 );
        float solid_color_a = m.getArgAsFloat( 3 );
        m_activeQuad->bgColor.r = solid_color_r;
        m_activeQuad->bgColor.g = solid_color_g;
        m_activeQuad->bgColor.b = solid_color_b;
        m_activeQuad->bgColor.a = solid_color_a;
    }

    else if ( messageAddress == "/active/solid/color/1" )
    {
        // arguments are f
        float solid_color_r = m.getArgAsFloat( 0 );
        m_activeQuad->bgColor.r = solid_color_r;
    }

    else if ( messageAddress == "/active/solid/color/2" )
    {
        // arguments are f
        float solid_color_g = m.getArgAsFloat( 0 );
        m_activeQuad->bgColor.g = solid_color_g;
    }

    else if ( messageAddress == "/active/solid/color/3" )
    {
        // arguments are f
        float solid_color_b = m.getArgAsFloat( 0 );
        m_activeQuad->bgColor.b = solid_color_b;
    }

    else if ( messageAddress == "/active/solid/color/4" )
    {
        // arguments are f
        float solid_color_a = m.getArgAsFloat( 0 );
        m_activeQuad->bgColor.a = solid_color_a;
    }

    else if ( messageAddress == "/active/solid/trans" )
    {
        m_activeQuad->transBg = !m_activeQuad->transBg;
    }

    // trans on/off
    else if ( messageAddress == "/active/solid/trans/show" )
    {
        // argument is int32
        int osc_quad_transBg = m.getArgAsInt32( 0 );
        if(osc_quad_transBg == 0)
        {
            m_activeQuad->transBg = false;
        }
        else if(osc_quad_transBg == 1)
        {
            m_activeQuad->transBg = true;
        }
    }

    else if ( messageAddress == "/active/solid/trans/color" )
    {
        // arguments are ffff
        float trans_color_r = m.getArgAsFloat( 0 );
        float trans_color_g = m.getArgAsFloat( 1 );
        float trans_color_b = m.getArgAsFloat( 2 );
        float trans_color_a = m.getArgAsFloat( 3 );
        m_activeQuad->secondColor.r = trans_color_r;
        m_activeQuad->secondColor.g = trans_color_g;
        m_activeQuad->secondColor.b = trans_color_b;
        m_activeQuad->secondColor.a = trans_color_a;
    }

    else if ( messageAddress == "/active/solid/trans/color/1" )
    {
        // arguments are f
        float trans_color_r = m.getArgAsFloat( 0 );
        m_activeQuad->secondColor.r = trans_color_r;
    }

    else if ( messageAddress == "/active/solid/trans/color/2" )
    {
        // arguments are f
        float trans_color_g = m.getArgAsFloat( 0 );
        m_activeQuad->secondColor.g = trans_color_g;
    }

    else if ( messageAddress == "/active/solid/trans/color/3" )
    {
        // arguments are f
        float trans_color_b = m.getArgAsFloat( 0 );
        m_activeQuad->secondColor.b = trans_color_b;
    }

    else if ( messageAddress == "/active/solid/trans/color/4" )
    {
        // arguments are f
        float trans_color_a = m.getArgAsFloat( 0 );
        m_activeQuad->secondColor.a = trans_color_a;
    }

    else if ( messageAddress == "/active/solid/trans/duration" )
    {
        // arguments are f
        float trans_duration = m.getArgAsFloat( 0 );
        m_activeQuad->transDuration = trans_duration;
    }


    // mask stuff on active quad
    else if ( messageAddress == "/active/mask" )
    {
        m_activeQuad->bMask = !m_activeQuad->bMask;
    }

    // mask on/off
    else if ( messageAddress == "/active/mask/show" )
    {
        // argument is int32
        int osc_quad_bMask = m.getArgAsInt32( 0 );
        if(osc_quad_bMask == 0)
        {
            m_activeQuad->bMask = false;
        }
        else if(osc_quad_bMask == 1)
        {
            m_activeQuad->bMask = true;
        }
    }

    // mask invert
    else if ( messageAddress == "/active/mask/invert" )
    {
        // argument is int32
        int osc_quad_maskInvert = m.getArgAsInt32( 0 );
        if(osc_quad_maskInvert == 0)
        {
            m_activeQuad->maskInvert = false;
        }
        else if(osc_quad_maskInvert == 1)
        {
            m_activeQuad->maskInvert = true;
        }
    }

    // deform stuff

    // deform on/off
    else if ( messageAddress == "/active/deform/show" )
    {
        // argument is int32
        int osc_quad_bDeform = m.getArgAsInt32( 0 );
        if(osc_quad_bDeform == 0)
        {
            m_activeQuad->bDeform = false;
        }
        else if(osc_quad_bDeform == 1)
        {
            m_activeQuad->bDeform = true;
        }
    }

    // deform bezier
    else if ( messageAddress == "/active/deform/bezier" )
    {
        // argument is int32
        int osc_quad_bBezier = m.getArgAsInt32( 0 );
        if(osc_quad_bBezier == 0)
        {
            m_activeQuad->bBezier = false;
        }
        else if(osc_quad_bBezier == 1)
        {
            m_activeQuad->bBezier = true;
        }
    }

    else if ( messageAddress == "/active/deform/bezier/spherize/light" )
    {
        // no argument
        m_activeQuad->bezierSpherize();
    }

    else if ( messageAddress == "/active/deform/bezier/spherize/strong" )
    {
        // no argument
        m_activeQuad->bezierSpherizeStrong();
    }

    else if ( messageAddress == "/active/deform/bezier/reset" )
    {
        // no argument
        m_activeQuad->resetBezier();
    }

    // deform grid
    else if ( messageAddress == "/active/deform/grid" )
    {
        // argument is int32
        int osc_quad_bGrid = m.getArgAsInt32( 0 );
        if(osc_quad_bGrid == 0)
        {
            m_activeQuad->bGrid = false;
        }
        else if(osc_quad_bGrid == 1)
        {
            m_activeQuad->bGrid = true;
        }
    }

    // deform grid rows
    else if ( messageAddress == "/active/deform/grid/rows" )
    {
        // argument is int32
        int osc_quad_gridRows = m.getArgAsInt32( 0 );
        if(osc_quad_gridRows >= 2 && osc_quad_gridRows <= 15)
        {
            m_activeQuad->gridRows = osc_quad_gridRows;
        }
    }

    // deform grid columns
    else if ( messageAddress == "/active/deform/grid/columns" )
    {
        // argument is int32
        int osc_quad_gridColumns = m.getArgAsInt32( 0 );
        if(osc_quad_gridColumns >= 2 && osc_quad_gridColumns <= 20)
        {
            m_activeQuad->gridColumns = osc_quad_gridColumns;
        }
    }

    // edge-blend stuff
    // edge-blend on/off
    else if ( messageAddress == "/active/edgeblend/show" )
    {
        // argument is int32
        int osc_quad_edgeBlendBool = m.getArgAsInt32( 0 );
        if(osc_quad_edgeBlendBool == 0)
        {
            m_activeQuad->edgeBlendBool = false;
        }
        else if(osc_quad_edgeBlendBool == 1)
        {
            m_activeQuad->edgeBlendBool = true;
        }
    }

    else if ( messageAddress == "/active/edgeblend/power" )
    {
        // argument is float
        float osc_quad_edgeBlendExponent = m.getArgAsFloat( 0 );
        m_activeQuad->edgeBlendExponent = osc_quad_edgeBlendExponent;
    }

    else if ( messageAddress == "/active/edgeblend/gamma" )
    {
        // argument is float
        float osc_quad_edgeBlendGamma = m.getArgAsFloat( 0 );
        m_activeQuad->edgeBlendGamma = osc_quad_edgeBlendGamma;
    }

    else if ( messageAddress == "/active/edgeblend/luminance" )
    {
        // argument is float
        float osc_quad_edgeBlendLuminance = m.getArgAsFloat( 0 );
        m_activeQuad->edgeBlendLuminance = osc_quad_edgeBlendLuminance;
    }

    else if ( messageAddress == "/active/edgeblend/amount/left" )
    {
        // argument is float
        float osc_quad_edgeBlendAmountSin = m.getArgAsFloat( 0 );
        m_activeQuad->edgeBlendAmountSin = osc_quad_edgeBlendAmountSin;
    }

    else if ( messageAddress == "/active/edgeblend/amount/right" )
    {
        // argument is float
        float osc_quad_edgeBlendAmountDx = m.getArgAsFloat( 0 );
        m_activeQuad->edgeBlendAmountDx = osc_quad_edgeBlendAmountDx;
    }

    else if ( messageAddress == "/active/edgeblend/amount/top" )
    {
        // argument is float
        float osc_quad_edgeBlendAmountTop = m.getArgAsFloat( 0 );
        m_activeQuad->edgeBlendAmountTop = osc_quad_edgeBlendAmountTop;
    }

    else if ( messageAddress == "/active/edgeblend/amount/bottom" )
    {
        // argument is float
        float osc_quad_edgeBlendAmountBottom = m.getArgAsFloat( 0 );
        m_activeQuad->edgeBlendAmountBottom = osc_quad_edgeBlendAmountBottom;
    }

    else if ( messageAddress == "/active/edgeblend/amount" )
    {
        // argument is ffff
        float osc_quad_edgeBlendAmountTop = m.getArgAsFloat( 0 );
        float osc_quad_edgeBlendAmountDx = m.getArgAsFloat( 1 );
        float osc_quad_edgeBlendAmountBottom = m.getArgAsFloat( 2 );
        float osc_quad_edgeBlendAmountSin = m.getArgAsFloat( 3 );
        m_activeQuad->edgeBlendAmountTop = osc_quad_edgeBlendAmountTop;
        m_activeQuad->edgeBlendAmountDx = osc_quad_edgeBlendAmountDx;
        m_activeQuad->edgeBlendAmountBottom = osc_quad_edgeBlendAmountBottom;
        m_activeQuad->edgeBlendAmountSin = osc_quad_edgeBlendAmountSin;
    }

    // content displacement
    // displacement X
    else if ( messageAddress == "/active/placement/x" )
    {
        // argument is int32
        int osc_quad_quadDispX = m.getArgAsInt32( 0 );
        m_activeQuad->quadDispX = osc_quad_quadDispX;
    }

    // displacement Y
    else if ( messageAddress == "/active/placement/y" )
    {
        // argument is int32
        int osc_quad_quadDispY = m.getArgAsInt32( 0 );
        m_activeQuad->quadDispY = osc_quad_quadDispY;
    }

    // displacement XY
    else if ( messageAddress == "/active/placement" )
    {
        // argument is int32 int32
        int osc_quad_quadDispX = m.getArgAsInt32( 0 );
        int osc_quad_quadDispY = m.getArgAsInt32( 1 );
        m_activeQuad->quadDispX = osc_quad_quadDispX;
        m_activeQuad->quadDispY = osc_quad_quadDispY;
    }

    // displacement W
    else if ( messageAddress == "/active/placement/w" )
    {
        // argument is int32
        int osc_quad_quadW = m.getArgAsInt32( 0 );
        m_activeQuad->quadW = osc_quad_quadW;
    }

    // displacement H
    else if ( messageAddress == "/active/placement/h" )
    {
        // argument is int32
        int osc_quad_quadH = m.getArgAsInt32( 0 );
        m_activeQuad->quadH = osc_quad_quadH;
    }

    // displacement WH
    else if ( messageAddress == "/active/placement/dimensions" )
    {
        // argument is int32 int32
        int osc_quad_quadW = m.getArgAsInt32( 0 );
        int osc_quad_quadH = m.getArgAsInt32( 1 );
        m_activeQuad->quadW = osc_quad_quadW;
        m_activeQuad->quadH = osc_quad_quadH;
    }

    // displacement reset
    else if ( messageAddress == "/active/placement/reset" )
    {
        // no argument
        m_activeQuad->resetDimensions();
        m_activeQuad->resetPlacement();
    }

    // video stuff on active quad
    else if ( messageAddress == "/active/video" )
    {
        m_activeQuad->videoBg = !m_activeQuad->videoBg;
    }

    // video on/off
    else if ( messageAddress == "/active/video/show" )
    {
        // argument is int32
        int osc_quad_videoBg = m.getArgAsInt32( 0 );
        if(osc_quad_videoBg == 0)
        {
            m_activeQuad->videoBg = false;
        }
        else if(osc_quad_videoBg == 1)
        {
            m_activeQuad->videoBg = true;
        }
    }

    // video load
    else if ( messageAddress == "/active/video/load" )
    {
        // no argument
        openVideoFile();
    }

    // video HFlip
    else if ( messageAddress == "/active/video/hmirror" )
    {
        // argument is int32
        int osc_quad_videoHFlip = m.getArgAsInt32( 0 );
        if(osc_quad_videoHFlip == 0)
        {
            m_activeQuad->videoHFlip = false;
        }
        else if(osc_quad_videoHFlip == 1)
        {
            m_activeQuad->videoHFlip = true;
        }
    }

    // video VFlip
    else if ( messageAddress == "/active/video/vmirror" )
    {
        // argument is int32
        int osc_quad_videoVFlip = m.getArgAsInt32( 0 );
        if(osc_quad_videoVFlip == 0)
        {
            m_activeQuad->videoVFlip = false;
        }
        else if(osc_quad_videoVFlip == 1)
        {
            m_activeQuad->videoVFlip = true;
        }
    }



    else if ( messageAddress == "/active/video/color" )
    {
        // arguments are ffff
        float video_color_r = m.getArgAsFloat( 0 );
        float video_color_g = m.getArgAsFloat( 1 );
        float video_color_b = m.getArgAsFloat( 2 );
        float video_color_a = m.getArgAsFloat( 3 );
        m_activeQuad->videoColorize.r = video_color_r;
        m_activeQuad->videoColorize.g = video_color_g;
        m_activeQuad->videoColorize.b = video_color_b;
        m_activeQuad->videoColorize.a = video_color_a;
    }

    else if ( messageAddress == "/active/video/color/1" )
    {
        // arguments are f
        float video_color_r = m.getArgAsFloat( 0 );
        m_activeQuad->videoColorize.r = video_color_r;
    }

    else if ( messageAddress == "/active/video/color/2" )
    {
        // arguments are f
        float video_color_g = m.getArgAsFloat( 0 );
        m_activeQuad->videoColorize.g = video_color_g;
    }

    else if ( messageAddress == "/active/video/color/3" )
    {
        // arguments are f
        float video_color_b = m.getArgAsFloat( 0 );
        m_activeQuad->videoColorize.b = video_color_b;
    }

    else if ( messageAddress == "/active/video/color/4" )
    {
        // arguments are f
        float video_color_a = m.getArgAsFloat( 0 );
        m_activeQuad->videoColorize.a = video_color_a;
    }

    else if ( messageAddress == "/active/video/mult/x" )
    {
        // arguments are f
        float video_mult_x = m.getArgAsFloat( 0 );
        m_activeQuad->videoMultX = video_mult_x;
    }

    else if ( messageAddress == "/active/video/mult/y" )
    {
        // arguments are f
        float video_mult_y = m.getArgAsFloat( 0 );
        m_activeQuad->videoMultY = video_mult_y;
    }

    else if ( messageAddress == "/active/video/speed" )
    {
        // arguments are f
        float video_speed = m.getArgAsFloat( 0 );
        m_activeQuad->videoSpeed = video_speed;
    }

    else if ( messageAddress == "/active/video/volume" )
    {
        // arguments are i
        int video_volume = m.getArgAsInt32( 0 );
        m_activeQuad->videoVolume = video_volume;
    }

    // video loop
    else if ( messageAddress == "/active/video/loop" )
    {
        // argument is int32
        int osc_quad_videoLoop = m.getArgAsInt32( 0 );
        if(osc_quad_videoLoop == 0)
        {
            m_activeQuad->videoLoop = false;
        }
        else if(osc_quad_videoLoop == 1)
        {
            m_activeQuad->videoLoop = true;
        }
    }

    // video greenscreen
    else if ( messageAddress == "/active/video/greenscreen" )
    {
        // argument is int32
        int osc_quad_videoGreenscreen = m.getArgAsInt32( 0 );
        if(osc_quad_videoGreenscreen == 0)
        {
            m_activeQuad->videoGreenscreen = false;
        }
        else if(osc_quad_videoGreenscreen == 1)
        {
            m_activeQuad->videoGreenscreen = true;
        }
    }

    // camera stuff

    else if ( messageAddress == "/active/cam" )
    {
        m_activeQuad->camBg = !m_activeQuad->camBg;
    }

    // video on/off
    else if ( messageAddress == "/active/cam/show" )
    {
        // argument is int32
        int osc_quad_camBg = m.getArgAsInt32( 0 );
        if(osc_quad_camBg == 0)
        {
            m_activeQuad->camBg = false;
        }
        else if(osc_quad_camBg == 1)
        {
            m_activeQuad->camBg = true;
        }
    }

    // video HFlip
    else if ( messageAddress == "/active/cam/hmirror" )
    {
        // argument is int32
        int osc_quad_camHFlip = m.getArgAsInt32( 0 );
        if(osc_quad_camHFlip == 0)
        {
            m_activeQuad->camHFlip = false;
        }
        else if(osc_quad_camHFlip == 1)
        {
            m_activeQuad->camHFlip = true;
        }
    }

    // video VFlip
    else if ( messageAddress == "/active/cam/vmirror" )
    {
        // argument is int32
        int osc_quad_camVFlip = m.getArgAsInt32( 0 );
        if(osc_quad_camVFlip == 0)
        {
            m_activeQuad->camVFlip = false;
        }
        else if(osc_quad_camVFlip == 1)
        {
            m_activeQuad->camVFlip = true;
        }
    }


    else if ( messageAddress == "/active/cam/color" )
    {
        // arguments are ffff
        float cam_color_r = m.getArgAsFloat( 0 );
        float cam_color_g = m.getArgAsFloat( 1 );
        float cam_color_b = m.getArgAsFloat( 2 );
        float cam_color_a = m.getArgAsFloat( 3 );
        m_activeQuad->camColorize.r = cam_color_r;
        m_activeQuad->camColorize.g = cam_color_g;
        m_activeQuad->camColorize.b = cam_color_b;
        m_activeQuad->camColorize.a = cam_color_a;
    }

    else if ( messageAddress == "/active/cam/color/1" )
    {
        // arguments are f
        float cam_color_r = m.getArgAsFloat( 0 );
        m_activeQuad->camColorize.r = cam_color_r;
    }

    else if ( messageAddress == "/active/cam/color/2" )
    {
        // arguments are f
        float cam_color_g = m.getArgAsFloat( 0 );
        m_activeQuad->camColorize.g = cam_color_g;
    }

    else if ( messageAddress == "/active/cam/color/3" )
    {
        // arguments are f
        float cam_color_b = m.getArgAsFloat( 0 );
        m_activeQuad->camColorize.b = cam_color_b;
    }

    else if ( messageAddress == "/active/cam/color/4" )
    {
        // arguments are f
        float cam_color_a = m.getArgAsFloat( 0 );
        m_activeQuad->camColorize.a = cam_color_a;
    }

    else if ( messageAddress == "/active/cam/mult/x" )
    {
        // arguments are f
        float cam_mult_x = m.getArgAsFloat( 0 );
        m_activeQuad->camMultX = cam_mult_x;
    }

    else if ( messageAddress == "/active/cam/mult/y" )
    {
        // arguments are f
        float cam_mult_y = m.getArgAsFloat( 0 );
        m_activeQuad->camMultY = cam_mult_y;
    }


    // cam greenscreen
    else if ( messageAddress == "/active/cam/greenscreen" )
    {
        // argument is int32
        int osc_quad_camGreenscreen = m.getArgAsInt32( 0 );
        if(osc_quad_camGreenscreen == 0)
        {
            m_activeQuad->camGreenscreen = false;
        }
        else if(osc_quad_camGreenscreen == 1)
        {
            m_activeQuad->camGreenscreen = true;
        }
    }

    // greenscreen stuff

    // greenscreen threshold
    else if ( messageAddress == "/active/greenscreen/threshold" )
    {
        // argument is int32
        int osc_quad_thresholdGreenscreen = m.getArgAsInt32( 0 );
        m_activeQuad->thresholdGreenscreen = osc_quad_thresholdGreenscreen;
    }

    else if ( messageAddress == "/active/greenscreen/color" )
    {
        // arguments are ffff
        float greenscreen_color_r = m.getArgAsFloat( 0 );
        float greenscreen_color_g = m.getArgAsFloat( 1 );
        float greenscreen_color_b = m.getArgAsFloat( 2 );
        float greenscreen_color_a = m.getArgAsFloat( 3 );
        m_activeQuad->colorGreenscreen.r = greenscreen_color_r;
        m_activeQuad->colorGreenscreen.g = greenscreen_color_g;
        m_activeQuad->colorGreenscreen.b = greenscreen_color_b;
        m_activeQuad->colorGreenscreen.a = greenscreen_color_a;
    }

    else if ( messageAddress == "/active/greenscreen/color/1" )
    {
        // arguments are f
        float greenscreen_color_r = m.getArgAsFloat( 0 );
        m_activeQuad->colorGreenscreen.r = greenscreen_color_r;
    }

    else if ( messageAddress == "/active/greenscreen/color/2" )
    {
        // arguments are f
        float greenscreen_color_g = m.getArgAsFloat( 0 );
        m_activeQuad->colorGreenscreen.g = greenscreen_color_g;
    }

    else if ( messageAddress == "/active/greenscreen/color/3" )
    {
        // arguments are f
        float greenscreen_color_b = m.getArgAsFloat( 0 );
        m_activeQuad->colorGreenscreen.b = greenscreen_color_b;
    }

    else if ( messageAddress == "/active/greenscreen/color/4" )
    {
        // arguments are f
        float greenscreen_color_a = m.getArgAsFloat( 0 );
        m_activeQuad->colorGreenscreen.a = greenscreen_color_a;
    }

    // slideshow stuff

    else if ( messageAddress == "/active/slideshow" )
    {
        m_activeQuad->slideshowBg = !m_activeQuad->slideshowBg;
    }

    // slideshow on/off
    else if ( messageAddress == "/active/slideshow/show" )
    {
        // argument is int32
        int osc_quad_slideshowBg = m.getArgAsInt32( 0 );
        if(osc_quad_slideshowBg == 0)
        {
            m_activeQuad->slideshowBg = false;
        }
        else if(osc_quad_slideshowBg == 1)
        {
            m_activeQuad->slideshowBg = true;
        }
    }

    else if ( messageAddress == "/active/slideshow/folder" )
    {
        // argument is int32
        int osc_quad_bgSlideshow = m.getArgAsInt32( 0 );
        if(osc_quad_bgSlideshow <= slideshowFolders.size())
        {
            m_activeQuad->bgSlideshow = osc_quad_bgSlideshow;
        }
    }

    else if ( messageAddress == "/active/slideshow/fit" )
    {
        // argument is int32
        int osc_quad_slideFit = m.getArgAsInt32( 0 );
        if(osc_quad_slideFit == 0)
        {
            m_activeQuad->slideFit = false;
        }
        else if(osc_quad_slideFit == 1)
        {
            m_activeQuad->slideFit = true;
        }
    }

    else if ( messageAddress == "/active/slideshow/keep_aspect" )
    {
        // argument is int32
        int osc_quad_slideKeepAspect = m.getArgAsInt32( 0 );
        if(osc_quad_slideKeepAspect == 0)
        {
            m_activeQuad->slideKeepAspect = false;
        }
        else if(osc_quad_slideKeepAspect == 1)
        {
            m_activeQuad->slideKeepAspect = true;
        }
    }

    else if ( messageAddress == "/active/slideshow/duration" )
    {
        // arguments are f
        float osc_quad_slideshowSpeed = m.getArgAsFloat( 0 );
        m_activeQuad->slideshowSpeed = osc_quad_slideshowSpeed;
    }


    // kinect stuff
    else if ( messageAddress == "/active/kinect" )
    {
        m_activeQuad->kinectBg = !m_activeQuad->kinectBg;
    }

    // slideshow on/off
    else if ( messageAddress == "/active/kinect/show" )
    {
        // argument is int32
        int osc_quad_kinectBg = m.getArgAsInt32( 0 );
        if(osc_quad_kinectBg == 0)
        {
            m_activeQuad->kinectBg = false;
        }
        else if(osc_quad_kinectBg == 1)
        {
            m_activeQuad->kinectBg = true;
        }
    }

    //kinect stuff
#ifdef WITH_KINECT
    else if ( messageAddress == "/active/kinect/open" )
    {
        // no argument
        kinect.kinect.open();
    }

    else if ( messageAddress == "/active/kinect/close" )
    {
        // no argument
        kinect.kinect.setCameraTiltAngle(0);
        kinect.kinect.close();
    }

    // kinect image
    else if ( messageAddress == "/active/kinect/show/image" )
    {
        // argument is int32
        int osc_quad_kinectImg = m.getArgAsInt32( 0 );
        if(osc_quad_kinectImg == 0)
        {
            m_activeQuad->kinectImg = false;
        }
        else if(osc_quad_kinectImg == 1)
        {
            m_activeQuad->kinectImg = true;
        }
    }

    // kinect grayscale image
    else if ( messageAddress == "/active/kinect/show/grayscale" )
    {
        // argument is int32
        int osc_quad_getKinectGrayImage = m.getArgAsInt32( 0 );
        if(osc_quad_getKinectGrayImage == 0)
        {
            m_activeQuad->getKinectGrayImage = false;
        }
        else if(osc_quad_getKinectGrayImage == 1)
        {
            m_activeQuad->getKinectGrayImage = true;
        }
    }

    else if ( messageAddress == "/active/kinect/mask" )
    {
        // argument is int32
        int osc_quad_kinectMask = m.getArgAsInt32( 0 );
        if(osc_quad_kinectMask == 0)
        {
            m_activeQuad->kinectMask = false;
        }
        else if(osc_quad_kinectMask == 1)
        {
            m_activeQuad->kinectMask = true;
        }
    }


    else if ( messageAddress == "/active/kinect/mult/x" )
    {
        // arguments are f
        float kinect_mult_x = m.getArgAsFloat( 0 );
        m_activeQuad->kinectMultX = kinect_mult_x;
    }

    else if ( messageAddress == "/active/kinect/mult/y" )
    {
        // arguments are f
        float kinect_mult_y = m.getArgAsFloat( 0 );
        m_activeQuad->kinectMultY = kinect_mult_y;
    }

    else if ( messageAddress == "/active/kinect/scale/x" )
    {
        // arguments are f
        float kinect_mult_x = m.getArgAsFloat( 0 );
        m_activeQuad->kinectMultX = kinect_mult_x;
    }

    else if ( messageAddress == "/active/kinect/scale/y" )
    {
        // arguments are f
        float kinect_mult_y = m.getArgAsFloat( 0 );
        m_activeQuad->kinectMultY = kinect_mult_y;
    }


    else if ( messageAddress == "/active/kinect/threshold/near" )
    {
        // arguments are int32
        int osc_quad_nearDepthTh = m.getArgAsInt32( 0 );
        m_activeQuad->nearDepthTh = osc_quad_nearDepthTh;
    }

    else if ( messageAddress == "/active/kinect/threshold/far" )
    {
        // arguments are int32
        int osc_quad_farDepthTh = m.getArgAsInt32( 0 );
        m_activeQuad->farDepthTh = osc_quad_farDepthTh;
    }

    else if ( messageAddress == "/active/kinect/angle" )
    {
        // argument is int32
        int osc_quad_kinectAngle = m.getArgAsInt32( 0 );
        if(osc_quad_kinectAngle >= -30 && osc_quad_kinectAngle <= 30)
        {
            kinect.kinectAngle = osc_quad_kinectAngle;
        }
    }

    else if ( messageAddress == "/active/kinect/blur" )
    {
        // argument is int32
        int osc_quad_kinectBlur = m.getArgAsInt32( 0 );
        if(osc_quad_kinectBlur >= 0 && osc_quad_kinectBlur <= 10)
        {
            m_activeQuad->kinectBlur = osc_quad_kinectBlur;
        }
    }

    else if ( messageAddress == "/active/kinect/contour" )
    {
        // argument is int32
        int osc_quad_getKinectContours = m.getArgAsInt32( 0 );
        if(osc_quad_getKinectContours == 0)
        {
            m_activeQuad->getKinectContours = false;
        }
        else if(osc_quad_getKinectContours == 1)
        {
            m_activeQuad->getKinectContours = true;
        }
    }

    else if ( messageAddress == "/active/kinect/contour/curves" )
    {
        // argument is int32
        int osc_quad_kinectContourCurved = m.getArgAsInt32( 0 );
        if(osc_quad_kinectContourCurved == 0)
        {
            m_activeQuad->kinectContourCurved = false;
        }
        else if(osc_quad_kinectContourCurved == 1)
        {
            m_activeQuad->kinectContourCurved = true;
        }
    }

    else if ( messageAddress == "/active/kinect/contour/smooth" )
    {
        // argument is int32
        int osc_quad_kinectContourSmooth = m.getArgAsInt32( 0 );
        if(osc_quad_kinectContourSmooth >= 0 && osc_quad_kinectContourSmooth <= 20)
        {
            m_activeQuad->kinectContourSmooth = osc_quad_kinectContourSmooth;
        }
    }

    else if ( messageAddress == "/active/kinect/contour/simplify" )
    {
        // argument is f
        float osc_quad_kinectContourSimplify = m.getArgAsFloat( 0 );
        m_activeQuad->kinectContourSimplify = osc_quad_kinectContourSimplify;
    }

    else if ( messageAddress == "/active/kinect/contour/area/min" )
    {
        // argument is f
        float osc_quad_kinectContourMin = m.getArgAsFloat( 0 );
        m_activeQuad->kinectContourMin = osc_quad_kinectContourMin;
    }

    else if ( messageAddress == "/active/kinect/contour/area/max" )
    {
        // argument is f
        float osc_quad_kinectContourMax = m.getArgAsFloat( 0 );
        m_activeQuad->kinectContourMax = osc_quad_kinectContourMax;
    }

    else if ( messageAddress == "/active/kinect/contour/area" )
    {
        // argument is f f
        float osc_quad_kinectContourMin = m.getArgAsFloat( 0 );
        float osc_quad_kinectContourMax = m.getArgAsFloat( 1 );
        m_activeQuad->kinectContourMin = osc_quad_kinectContourMin;
        m_activeQuad->kinectContourMax = osc_quad_kinectContourMax;
    }

    else if ( messageAddress == "/active/kinect/color" )
    {
        // arguments are ffff
        float kinect_color_r = m.getArgAsFloat( 0 );
        float kinect_color_g = m.getArgAsFloat( 1 );
        float kinect_color_b = m.getArgAsFloat( 2 );
        float kinect_color_a = m.getArgAsFloat( 3 );
        m_activeQuad->kinectColorize.r = kinect_color_r;
        m_activeQuad->kinectColorize.g = kinect_color_g;
        m_activeQuad->kinectColorize.b = kinect_color_b;
        m_activeQuad->kinectColorize.a = kinect_color_a;
    }

    else if ( messageAddress == "/active/kinect/color/1" )
    {
        // arguments are f
        float kinect_color_r = m.getArgAsFloat( 0 );
        m_activeQuad->kinectColorize.r = kinect_color_r;
    }

    else if ( messageAddress == "/active/kinect/color/2" )
    {
        // arguments are f
        float kinect_color_g = m.getArgAsFloat( 0 );
        m_activeQuad->kinectColorize.g = kinect_color_g;
    }

    else if ( messageAddress == "/active/kinect/color/3" )
    {
        // arguments are f
        float kinect_color_b = m.getArgAsFloat( 0 );
        m_activeQuad->kinectColorize.b = kinect_color_b;
    }

    else if ( messageAddress == "/active/kinect/color/4" )
    {
        // arguments are f
        float kinect_color_a = m.getArgAsFloat( 0 );
        m_activeQuad->kinectColorize.a = kinect_color_a;
    }
#endif

    else if ( messageAddress == "/active/crop/rectangular/top" )
    {
        // arguments are f
        float crop_top = m.getArgAsFloat(0);
        m_activeQuad->crop[0] = crop_top;
    }
    else if ( messageAddress == "/active/crop/rectangular/right" )
    {
        // arguments are f
        float crop_right = m.getArgAsFloat(0);
        m_activeQuad->crop[1] = crop_right;
    }
    else if ( messageAddress == "/active/crop/rectangular/bottom" )
    {
        // arguments are f
        float crop_bottom = m.getArgAsFloat(0);
        m_activeQuad->crop[2] = crop_bottom;
    }
    else if ( messageAddress == "/active/crop/rectangular/left" )
    {
        // arguments are f
        float crop_left = m.getArgAsFloat(0);
        m_activeQuad->crop[3] = crop_left;
    }
    else if ( messageAddress == "/active/crop/circular/x" )
    {
        // arguments are f
        float crop_center_x = m.getArgAsFloat(0);
        m_activeQuad->circularCrop[0] = crop_center_x;
    }
    else if ( messageAddress == "/active/crop/circular/y" )
    {
        // arguments are f
        float crop_center_y = m.getArgAsFloat(0);
        m_activeQuad->circularCrop[1] = crop_center_y;
    }
    else if ( messageAddress == "/active/crop/circular/radius" )
    {
        // arguments are f
        float crop_radius = m.getArgAsFloat(0);
        m_activeQuad->circularCrop[2] = crop_radius;
    }

    /*
    else
    {
        // unrecognized message: display on the bottom of the screen
        string msg_string;
        msg_string = messageAddress;
        msg_string += ": ";
        for ( int i=0; i<m.getNumArgs(); i++ )
        {
            // get the argument type
            msg_string += m.getArgTypeName( i );
            msg_string += ":";
            // display the argument - make sure we get the right type
            if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
                msg_string += ofToString( m.getArgAsInt32( i ) );
            else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
                msg_string += ofToString( m.getArgAsFloat( i ) );
            else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
                msg_string += m.getArgAsString( i );
            else
                msg_string += "unknown";
        }
        // add to the list of strings to display
        msg_strings[current_msg_string] = msg_string;
        timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
        current_msg_string = ( current_msg_string + 1 ) % NUM_MSG_STRINGS;
        // clear the next line
        msg_strings[current_msg_string] = "";
        cout << endl << msg_string << endl;
    }
    */
    ///*

    // if we get an OSC message not recognized, we can use it for auto-learning gui control as with midi
    else
    {
        // hotkey stuff - learning
        if(bMidiHotkeyCoupling && midiHotkeyPressed >= 0)
        {
            if(oscHotkeyMessages.size()>0 && oscHotkeyMessages.size() == oscHotkeyKeys.size())
            {
                for(int i=0; i < oscHotkeyMessages.size(); i++)
                {
                    // check if we already have a message for selected hotkey and eventually removes it
                    if(oscHotkeyKeys[i] == midiHotkeyPressed)
                    {
                        oscHotkeyKeys.erase(oscHotkeyKeys.begin()+i);
                        oscHotkeyMessages.erase(oscHotkeyMessages.begin()+i);
                    }
                }
            }
            oscHotkeyMessages.push_back(m);
            oscHotkeyKeys.push_back(midiHotkeyPressed);
            cout << endl << "OSC message '" << messageAddress << " " << m.getArgAsString(0) << "' coupled to hotkey '" << (char) midiHotkeyPressed << "'" << endl;
            midiHotkeyPressed = -1;
            bMidiHotkeyCoupling = false;
            bMidiHotkeyLearning = false;
            return;
        }

        // hotkey stuff - checking
        if(oscHotkeyMessages.size()>0 && oscHotkeyMessages.size() == oscHotkeyKeys.size())
        {
            bool keyFound = false;
            for(int i=0; i < oscHotkeyMessages.size(); i++)
            {
                ofxOscMessage oscControl = oscHotkeyMessages[i];
                if(messageAddress == oscControl.getAddress())
                {
                    if(m.getNumArgs()>0 && m.getArgType(0) == OFXOSC_TYPE_INT32)
                    {
                        if(m.getArgAsInt32(0) == oscControl.getArgAsInt32(0))
                        {
                            keyPressed(oscHotkeyKeys[i]);
                            keyFound = true;
                        }
                    }
                    else if(m.getNumArgs()>0 && m.getArgType(0) == OFXOSC_TYPE_FLOAT)
                    {
                        if(m.getArgAsFloat(0) == oscControl.getArgAsFloat(0))
                        {
                            keyPressed(oscHotkeyKeys[i]);
                            keyFound = true;
                        }
                    }
                    else if(m.getNumArgs()>0 && m.getArgType(0) == OFXOSC_TYPE_STRING)
                    {
                        if(m.getArgAsString(0) == oscControl.getArgAsString(0))
                        {
                            keyPressed(oscHotkeyKeys[i]);
                            keyFound = true;
                        }
                    }
                    else if(m.getNumArgs()==0)
                    {
                        keyPressed(oscHotkeyKeys[i]);
                        keyFound = true;
                    }
                }
            }
            if(keyFound)
                return;
        }

        // gui coupling stuff
        for(int i=0; i < m_gui.getPages().size(); i++)
        {
            for(int j=0; j < m_gui.getPages()[i]->getControls().size(); j++)
            {
                // toggle case
                if(m_gui.getPages()[i]->getControls()[j]->controlType == "Toggle")
                {
                    // learning
                    if(m_gui.getPages()[i]->getControls()[j]->bLearning)
                    {
                        m_gui.getPages()[i]->getControls()[j]->bLearning = false;
                        m_gui.getPages()[i]->getControls()[j]->bLearnt = true;
                        m_gui.getPages()[i]->getControls()[j]->oscControl = m;
                    }
                    // checking
                    else if(m_gui.getPages()[i]->getControls()[j]->bLearnt)
                    {
                        ofxOscMessage oscControl = m_gui.getPages()[i]->getControls()[j]->oscControl;
                        if(messageAddress == oscControl.getAddress())
                        {
                            if(m.getNumArgs()>0 && m.getArgType(0) == OFXOSC_TYPE_INT32)
                            {
                                if(m.getArgAsInt32(0) == oscControl.getArgAsInt32(0))
                                {
                                    ofxSimpleGuiToggle *t = (ofxSimpleGuiToggle *) m_gui.getPages()[i]->getControls()[j];
                                    t->toggle();
                                }
                            }
                            else if(m.getNumArgs()>0 && m.getArgType(0) == OFXOSC_TYPE_FLOAT)
                            {
                                if(m.getArgAsFloat(0) == oscControl.getArgAsFloat(0))
                                {
                                    ofxSimpleGuiToggle *t = (ofxSimpleGuiToggle *) m_gui.getPages()[i]->getControls()[j];
                                    t->toggle();
                                }
                            }
                            else if(m.getNumArgs()>0 && m.getArgType(0) == OFXOSC_TYPE_STRING)
                            {
                                if(m.getArgAsString(0) == oscControl.getArgAsString(0))
                                {
                                    ofxSimpleGuiToggle *t = (ofxSimpleGuiToggle *) m_gui.getPages()[i]->getControls()[j];
                                    t->toggle();
                                }
                            }
                            else if(m.getNumArgs()==0)
                            {
                                ofxSimpleGuiToggle *t = (ofxSimpleGuiToggle *) m_gui.getPages()[i]->getControls()[j];
                                t->toggle();
                            }
                        }

                    }
                }
                // slider case
                else if(m_gui.getPages()[i]->getControls()[j]->controlType == "SliderFloat" || m_gui.getPages()[i]->getControls()[j]->controlType == "SliderInt")
                {
                    // learning
                    if(m_gui.getPages()[i]->getControls()[j]->bLearning)
                    {
                        if(m.getNumArgs()>0)
                        {
                            m_gui.getPages()[i]->getControls()[j]->bLearning = false;
                            m_gui.getPages()[i]->getControls()[j]->bLearnt = true;
                            m_gui.getPages()[i]->getControls()[j]->oscControl = m;
                        }
                    }
                    // checking
                    else if(m_gui.getPages()[i]->getControls()[j]->bLearnt)
                    {
                        ofxOscMessage oscControl = m_gui.getPages()[i]->getControls()[j]->oscControl;
                        if(m.getNumArgs()>0 && (m.getArgType(0) == OFXOSC_TYPE_INT32 || m.getArgType(0) == OFXOSC_TYPE_FLOAT))
                        {
                            if(messageAddress == oscControl.getAddress())
                            {
                                if(m_gui.getPages()[i]->getControls()[j]->controlType == "SliderFloat")
                                {
                                    ofxSimpleGuiSliderFloat *s = (ofxSimpleGuiSliderFloat *) m_gui.getPages()[i]->getControls()[j];
                                    float remappedValue;
                                    if(m.getArgType(0) == OFXOSC_TYPE_INT32)
                                    {
                                        float value = (float)m.getArgAsInt32(0);
                                        remappedValue = ofMap(value, (float) oscControlMin, (float) oscControlMax, (float) s->min, (float) s->max);
                                    }
                                    else
                                    {
                                        float value = m.getArgAsFloat(0);
                                        remappedValue = ofMap(value, (float) oscControlMin, (float) oscControlMax, (float) s->min, (float) s->max);
                                    }
                                    s->setValue(remappedValue);
                                }
                                else
                                {
                                    ofxSimpleGuiSliderInt *s = (ofxSimpleGuiSliderInt *) m_gui.getPages()[i]->getControls()[j];
                                    float remappedValue;
                                    if(m.getArgType(0) == OFXOSC_TYPE_INT32)
                                    {
                                        float value = (float)m.getArgAsInt32(0);
                                        remappedValue = ofMap(value, (float) oscControlMin, (float) oscControlMax, (float) s->min, (float) s->max);
                                    }
                                    else
                                    {
                                        float value = m.getArgAsFloat(0);
                                        remappedValue = ofMap(value, (float) oscControlMin, (float) oscControlMax, (float) s->min, (float) s->max);
                                    }
                                    s->setValue((int)remappedValue);
                                }
                            }
                        }
                    }
                }
            }
        }

    }//*/

}
