#include "ofApp.h"

void ofApp::setup() {
    seamCarver.setup();
    imageLoaded = false;
    isDragging = false;
    showEnergyMap = false;
    messageAlpha = 0;
}

void ofApp::update() {
    seamCarver.update();
    
    // 메시지 페이드 아웃
    if (messageAlpha > 0) {
        messageAlpha -= 0.5;
    }
}

void ofApp::draw() {
    ofBackground(40);
    
    if (imageLoaded) {
        // 기본 이미지 또는 에너지맵 그리기
        if (showEnergyMap) {
            seamCarver.drawEnergyMap(resizeHandle.x, resizeHandle.y, 
                                    resizeHandle.width, resizeHandle.height);
        } else {
            seamCarver.getResultImage().draw(resizeHandle);
        }
        
        // 테두리 그리기
        ofNoFill();
        ofSetColor(255, 0, 0);
        ofSetLineWidth(2);
        ofDrawRectangle(resizeHandle);
        
        // 상태 메시지 표시
        if (messageAlpha > 0) {
            ofSetColor(255, 255, 255, messageAlpha);
            ofDrawBitmapString(statusMessage, 
                            ofGetWidth()/2 - 100, 
                            ofGetHeight() - 50);
        }
        
        // 이미지 크기 정보
        ofSetColor(255);
        string sizeInfo = "Image Size: " + 
                       ofToString(seamCarver.getResultImage().getWidth()) + " x " + 
                       ofToString(seamCarver.getResultImage().getHeight()) + " px";
        ofDrawBitmapString(sizeInfo, 10, 20);
        
        // 도움말
        ofDrawBitmapString("Press 'e' to toggle energy map", 10, 40);
        ofDrawBitmapString("Press 's' to save result", 10, 60);
    } else {
        // 이미지 로드 안내 메시지
        ofSetColor(200);
        string msg = "Drag and drop an image file to start";
        ofDrawBitmapString(msg, 
                        ofGetWidth()/2 - 100, 
                        ofGetHeight()/2);
    }
}

void ofApp::keyPressed(int key) {
    if (key == 'e' || key == 'E') {
        showEnergyMap = !showEnergyMap;
    }
    
    if (key == 's' || key == 'S') {
        if (imageLoaded) {
            seamCarver.saveResultImage("result_seam_carved.png");
            statusMessage = "Image saved as 'result_seam_carved.png'";
            messageAlpha = 255;
        }
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if (imageLoaded && resizeHandle.inside(x, y)) {
        isDragging = true;
        dragStart = glm::vec2(x, y);
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    if (isDragging) {
        float deltaX = x - dragStart.x;
        float deltaY = y - dragStart.y;
        
        resizeHandle.width += deltaX;
        resizeHandle.height += deltaY;
        
        // 최소 크기 제한
        resizeHandle.width = max(100.0f, resizeHandle.width);
        resizeHandle.height = max(100.0f, resizeHandle.height);
        
        dragStart = glm::vec2(x, y);
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    if (isDragging) {
        isDragging = false;
        
        // 애니메이션 없이 즉시 리사이징
        seamCarver.resizeTo(resizeHandle.width, resizeHandle.height);
        
        statusMessage = "Image resized using Seam Carving";
        messageAlpha = 255;
    }
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    if (dragInfo.files.size() > 0) {
        if (seamCarver.loadImage(dragInfo.files[0])) {
            imageLoaded = true;
            
            // 이미지 중앙 정렬
            centerImage();
            
            statusMessage = "Image loaded successfully";
            messageAlpha = 255;
        }
    }
}

void ofApp::centerImage() {
    if (imageLoaded) {
        ofImage& img = seamCarver.getResultImage();
        float windowWidth = ofGetWidth();
        float windowHeight = ofGetHeight();
        float imageWidth = img.getWidth();
        float imageHeight = img.getHeight();
        
        // 이미지가 창보다 큰 경우 크기를 조정
        if (imageWidth > windowWidth || imageHeight > windowHeight) {
            float scale = min(windowWidth / imageWidth, windowHeight / imageHeight) * 0.8;
            imageWidth *= scale;
            imageHeight *= scale;
        }
        
        // 중앙 위치 계산
        float x = (windowWidth - imageWidth) / 2;
        float y = (windowHeight - imageHeight) / 2;
        
        resizeHandle.set(x, y, imageWidth, imageHeight);
    }
}