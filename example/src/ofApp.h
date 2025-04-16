#pragma once

#include "ofMain.h"
#include "ofxSeamCarving.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    
    void keyPressed(int key) override;
    void mousePressed(int x, int y, int button) override;
    void mouseDragged(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void dragEvent(ofDragInfo dragInfo) override;
    
private:
    ofxSeamCarving seamCarver;
    
    // UI 관련 변수
    ofRectangle resizeHandle;
    bool isDragging;
    bool imageLoaded;
    glm::vec2 dragStart;
    
    // 상태 메시지
    string statusMessage;
    float messageAlpha;
    
    // 시각화 상태 
    bool showEnergyMap;
    
    void centerImage();
};