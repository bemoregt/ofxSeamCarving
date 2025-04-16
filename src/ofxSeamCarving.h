#pragma once

#include "ofMain.h"

class ofxSeamCarving {
public:
    ofxSeamCarving();
    ~ofxSeamCarving();
    
    // 메인 함수들
    void setup();
    void update();
    void draw();
    
    // 이미지 로딩
    bool loadImage(const string& path);
    bool setImage(const ofImage& img);
    
    // 리사이징 함수들
    void resizeToWidth(int targetWidth);
    void resizeToHeight(int targetHeight);
    void resizeTo(int targetWidth, int targetHeight);
    void resizeWithCallback(int targetWidth, int targetHeight, bool animated = false);
    
    // 결과 관련 함수들
    ofImage& getResultImage();
    void saveResultImage(const string& path);
    
    // 시각화 관련 함수들
    void enableVisualization(bool enable);
    void drawEnergyMap(float x, float y, float width, float height);
    void drawWithSeam(float x, float y, float width, float height);
    
    // 이벤트 콜백
    ofEvent<void> resizeComplete;
    
private:
    // 알고리즘 구현 함수들
    vector<vector<float>> calculateEnergy(const ofImage& img);
    vector<int> findVerticalSeam(const vector<vector<float>>& energy);
    void removeVerticalSeam(ofImage& img, const vector<int>& seam);
    void resizeWithSeamCarving(ofImage& img, int targetWidth, int targetHeight, bool animated = false);
    
    // 이미지 데이터
    ofImage originalImage;
    ofImage workingImage;
    ofImage energyMapImage;
    
    // 시각화 관련 변수
    bool visualizationEnabled;
    vector<int> currentSeam;
    
    // 애니메이션 관련 변수
    bool isAnimating;
    int currentStep;
    int totalSteps;
    int targetWidth;
    int targetHeight;
};