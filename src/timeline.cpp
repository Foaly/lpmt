#include "testApp.h"
#ifdef WITH_TIMELINE
//--------------------------------------------------------------
void testApp::timelineSetup(float duration){

    timeline.setup();
    timeline.setSpacebarTogglePlay(false);
    timeline.setWorkingFolder("timeline");
    timeline.setDurationInSeconds(duration);
    timeline.setPageName("main"); //changes the first page name
    timeline.addFlags("trigger_main", "main_trigger.xml");

    for(int i = 0; i < 4; i++)
    {
        timelineAddQuadPage(i);
    }

    timeline.setLoopType(OF_LOOP_NORMAL);
    //timeline.enableSnapToBPM(60.0);
    timeline.enableSnapToOtherKeyframes(false);
    timeline.setEditableHeaders(true);
    //timeline.collapseAllTracks();
    ofAddListener(timeline.events().bangFired, this, &testApp::timelineTriggerReceived);
}

//--------------------------------------------------------------
void testApp::timelineUpdate()
{
    for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
    {
        if(quad->bTimelineTint)
        {
            quad->timelineRed = timeline.getValue("red_" + ofToString(quad->quadNumber));
            quad->timelineGreen = timeline.getValue("green_" + ofToString(quad->quadNumber));
            quad->timelineBlu = timeline.getValue("blu_" + ofToString(quad->quadNumber));
        }
        if(quad->bTimelineColor)
        {
            quad->timelineColor = timeline.getColor("color_" + ofToString(quad->quadNumber));
            quad->bgColor = quad->timelineColor;
        }
        if(quad->bTimelineAlpha)
        {
            quad->timelineAlpha = timeline.getValue("alpha_" + ofToString(quad->quadNumber));
        }
    }
}

//--------------------------------------------------------------
void testApp::timelineTriggerReceived(ofxTLBangEventArgs& trigger){
    //vector<string> triggerParts = ofSplitString(trigger.triggerGroupName, "_", true, true);
    vector<string> triggerParts = ofSplitString(trigger.track->getName(), "_", true, true);

    if(useTimeline)
    {
        //cout << "Trigger from " << trigger.triggerGroupName << " says color " << trigger.triggerName << endl;
        //cout << "Trigger from " << ofToInt(triggerParts[1]) << " says " << trigger.triggerName << endl;

        //string tlMsg = trigger.triggerName;
        string tlMsg = trigger.flag;
        string tlMsgParameter = "";

        if(triggerParts[1] != "main")
        {
        int tlQuad = ofToInt(triggerParts[1]);

        if(tlQuad >= m_quads.size())
        {
            tlQuad = m_quads.size() - 1;
        }
        else if(tlQuad < 0)
        {
            tlQuad = 0;
        }

        //check if we have a message with a parameter, parameters are given using a colon ':' as separator
        if (ofIsStringInString(tlMsg,":"))
        {
            vector<string> tlMsgParts = ofSplitString(tlMsg, ":", true, true);
            tlMsg = tlMsgParts[0];
            tlMsgParameter = tlMsgParts[1];
        }

        if (tlMsg == "on"){ m_quads[tlQuad].isOn=true; }
        else if (tlMsg == "off"){ m_quads[tlQuad].isOn=false; }
        else if(tlMsg == "img_on"){ m_quads[tlQuad].imgBg=true; }
        else if (tlMsg == "img_off"){ m_quads[tlQuad].imgBg=false; }
        else if (tlMsg == "col_on"){ m_quads[tlQuad].colorBg=true; }
        else if (tlMsg == "col_off"){ m_quads[tlQuad].colorBg=false; }
        else if (tlMsg == "video_on"){ m_quads[tlQuad].videoBg=true; }
        else if (tlMsg == "video_off"){ m_quads[tlQuad].videoBg=false; }
        else if (tlMsg == "video_stop"){ m_quads[tlQuad].video.stop(); }
        else if (tlMsg == "video_play"){ m_quads[tlQuad].video.play(); }
        else if (tlMsg == "video_reset"){ m_quads[tlQuad].video.setPosition(0.0); }
        else if (tlMsg == "video_position" && tlMsgParameter != ""){ m_quads[tlQuad].video.setPosition(ofToFloat(tlMsgParameter));}
        else if (tlMsg == "shared_video_on"){ m_quads[tlQuad].sharedVideoBg=true; }
        else if (tlMsg == "shared_video_off"){ m_quads[tlQuad].sharedVideoBg=false; }
        else if (tlMsg == "shared_video_num" && tlMsgParameter != ""){ m_quads[tlQuad].sharedVideoNum=ofToInt(tlMsgParameter); }
        else if (tlMsg == "slide_on"){ m_quads[tlQuad].slideshowBg=true; }
        else if (tlMsg == "slide_off"){ m_quads[tlQuad].slideshowBg=false; }
        else if (tlMsg == "slide_new"){ m_quads[tlQuad].currentSlide+=1; }
        else if (tlMsg == "slide_num" && tlMsgParameter != ""){ m_quads[tlQuad].currentSlide=ofToInt(tlMsgParameter); }
        else if (tlMsg == "cam_on"){ m_quads[tlQuad].camBg=true; }
        else if (tlMsg == "cam_off"){ m_quads[tlQuad].camBg=false; }
        else if (tlMsg == "kinect_on"){ m_quads[tlQuad].kinectBg=true; }
        else if (tlMsg == "kinect_off"){ m_quads[tlQuad].kinectBg=false; }
        else if (tlMsg == "mask_on"){ m_quads[tlQuad].bMask=true; }
        else if (tlMsg == "mask_off"){ m_quads[tlQuad].bMask=false; }
        else if (tlMsg == "mask_invert_on"){ m_quads[tlQuad].maskInvert=true; }
        else if (tlMsg == "mask_invert_off"){ m_quads[tlQuad].maskInvert=false; }
        else { cout << "unknown trigger command '" << tlMsg << "' on surface " << tlQuad << endl;}
        }
        else
        {
            // main timeline page
            //check if we have a message with a parameter, parameters are given using a colon ':' as separator
            if (ofIsStringInString(tlMsg,":"))
            {
                vector<string> tlMsgParts = ofSplitString(tlMsg, ":", true, true);
                tlMsg = tlMsgParts[0];
                tlMsgParameter = tlMsgParts[1];
            }

            // check messages
            if (tlMsg == "shared_videos_reset")
            {
                for(int j=0; j<4; j++)
                {
                    if(sharedVideos[j].isLoaded())
                    {
                        sharedVideos[j].setPosition(0.0);
                    }
                }
            }
            else if (tlMsg == "shared_video_reset" && tlMsgParameter != "")
            {
                if(ofToInt(tlMsgParameter) > 0 && ofToInt(tlMsgParameter) <= 4)
                {
                    if(sharedVideos[ofToInt(tlMsgParameter)-1].isLoaded())
                    {
                        sharedVideos[ofToInt(tlMsgParameter)-1].setPosition(0.0);
                    }

                }
            }
            else if (tlMsg == "videos_reset")
            {
                for(std::vector<Quad>::iterator quad = m_quads.begin(); quad < m_quads.end(); quad++)
                {
                    if(quad->video.isLoaded())
                    {
                        quad->video.setPosition(0.0);
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::timelineAddQuadPage(int i) {
    timeline.addPage(ofToString(i), true);
    timeline.addCurves("red_"+ofToString(i), ofToString(i)+"_red.xml", ofRange(0, 1.0));
	timeline.addCurves("green_"+ofToString(i), ofToString(i)+"_green.xml", ofRange(0, 1.0));
	timeline.addCurves("blu_"+ofToString(i), ofToString(i)+"_blu.xml", ofRange(0, 1.0));
	timeline.addCurves("alpha_"+ofToString(i), ofToString(i)+"_alpha.xml", ofRange(0, 1.0));
	timeline.addFlags("trigger_"+ofToString(i), ofToString(i)+"_trigger.xml");
	timeline.addColors("color_"+ofToString(i), ofToString(i)+"_color.xml");
}

#endif
