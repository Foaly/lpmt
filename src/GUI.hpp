#ifndef GUI_INCLUDE
#define GUI_INCLUDE

#include "ofxSimpleGuiToo.h"
#include "quad.h"

class testApp;

class GUI
{
public:
    GUI(testApp* app);
    void setupPages();
    void updatePages(quad& activeQuad);

private:
    ofxSimpleGuiConfig    m_config;
    testApp*              m_app;
    bool                  m_dummyBool;
    float                 m_dummyFloat;
    int                   m_dummyInt;
};

#endif // GUI_INCLUDE
