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
    void updatePages(Quad &activeQuad);

    void nextPage();
    void prevPage();
    void showPage(int i);               // 1 based index of page

    void toggleDraw();
    void show();
    void hide();
    bool isOn();

    std::vector<ofxSimpleGuiPage*>&    getPages();

    void draw();


private:
    ofxSimpleGuiToo       m_gui;
    ofxSimpleGuiConfig    m_config;
    testApp*              m_app;
    bool                  m_dummyBool;
    float                 m_dummyFloat;
    int                   m_dummyInt;
};

#endif // GUI_INCLUDE
