#include "ofxSeamCarving.h"

ofxSeamCarving::ofxSeamCarving() {
    visualizationEnabled = false;
    isAnimating = false;
    currentStep = 0;
    totalSteps = 0;
}

ofxSeamCarving::~ofxSeamCarving() {
    // 필요한 정리 작업
}

void ofxSeamCarving::setup() {
    // 초기화 코드
}

void ofxSeamCarving::update() {
    // 애니메이션 모드일 경우, 프레임마다 점진적으로 리사이징
    if (isAnimating && currentStep < totalSteps) {
        // 한 프레임에 하나의 심 제거
        vector<vector<float>> energy = calculateEnergy(workingImage);
        currentSeam = findVerticalSeam(energy);
        removeVerticalSeam(workingImage, currentSeam);
        
        currentStep++;
        
        // 완료 체크
        if (currentStep >= totalSteps) {
            isAnimating = false;
            ofNotifyEvent(resizeComplete);
        }
    }
}

void ofxSeamCarving::draw() {
    // 기본 그리기 기능은 비어 있음 - 사용자가 외부에서 호출하는 메서드 사용
}

bool ofxSeamCarving::loadImage(const string& path) {
    bool success = originalImage.load(path);
    if (success) {
        workingImage = originalImage;
        return true;
    }
    return false;
}

bool ofxSeamCarving::setImage(const ofImage& img) {
    originalImage = img;
    workingImage = originalImage;
    return true;
}

void ofxSeamCarving::resizeToWidth(int targetWidth) {
    if (targetWidth < workingImage.getWidth()) {
        resizeTo(targetWidth, workingImage.getHeight());
    }
}

void ofxSeamCarving::resizeToHeight(int targetHeight) {
    // 일시적으로 회전시켜 처리
    workingImage.rotate90(1);
    resizeToWidth(targetHeight);
    workingImage.rotate90(-1);
}

void ofxSeamCarving::resizeTo(int targetWidth, int targetHeight) {
    resizeWithSeamCarving(workingImage, targetWidth, targetHeight, false);
}

void ofxSeamCarving::resizeWithCallback(int targetWidth, int targetHeight, bool animated) {
    if (animated) {
        this->targetWidth = targetWidth;
        this->targetHeight = targetHeight;
        
        // 총 리사이징 단계 계산
        totalSteps = (workingImage.getWidth() - targetWidth) + 
                     (workingImage.getHeight() - targetHeight);
        
        currentStep = 0;
        isAnimating = true;
    } else {
        resizeTo(targetWidth, targetHeight);
        ofNotifyEvent(resizeComplete);
    }
}

ofImage& ofxSeamCarving::getResultImage() {
    return workingImage;
}

void ofxSeamCarving::saveResultImage(const string& path) {
    workingImage.save(path);
}

void ofxSeamCarving::enableVisualization(bool enable) {
    visualizationEnabled = enable;
}

void ofxSeamCarving::drawEnergyMap(float x, float y, float width, float height) {
    if (!workingImage.isAllocated()) return;
    
    // 에너지 맵 계산 및 시각화
    vector<vector<float>> energy = calculateEnergy(workingImage);
    
    // 에너지 맵을 이미지로 변환
    if (!energyMapImage.isAllocated() || 
        energyMapImage.getWidth() != workingImage.getWidth() || 
        energyMapImage.getHeight() != workingImage.getHeight()) {
        energyMapImage.allocate(workingImage.getWidth(), workingImage.getHeight(), OF_IMAGE_COLOR);
    }
    
    // 에너지 값 정규화 및 시각화
    float maxEnergy = 0;
    for (auto& row : energy) {
        for (auto e : row) {
            if (e > maxEnergy) maxEnergy = e;
        }
    }
    
    for (int y = 0; y < workingImage.getHeight(); y++) {
        for (int x = 0; x < workingImage.getWidth(); x++) {
            float normalizedEnergy = energy[y][x] / maxEnergy;
            ofColor c(normalizedEnergy * 255);
            energyMapImage.setColor(x, y, c);
        }
    }
    energyMapImage.update();
    
    // 그리기
    energyMapImage.draw(x, y, width, height);
}

void ofxSeamCarving::drawWithSeam(float x, float y, float width, float height) {
    if (!workingImage.isAllocated() || currentSeam.empty()) return;
    
    // 작업 이미지 복사본 생성
    ofImage displayImg = workingImage;
    
    // 현재 심을 빨간색으로 표시
    for (int y = 0; y < displayImg.getHeight(); y++) {
        int seamX = currentSeam[y];
        displayImg.setColor(seamX, y, ofColor(255, 0, 0));
    }
    displayImg.update();
    
    // 그리기
    displayImg.draw(x, y, width, height);
}

// 에너지 맵 계산
vector<vector<float>> ofxSeamCarving::calculateEnergy(const ofImage& img) {
    int width = img.getWidth();
    int height = img.getHeight();
    vector<vector<float>> energy(height, vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            ofColor left = (x > 0) ? img.getColor(x-1, y) : img.getColor(x, y);
            ofColor right = (x < width-1) ? img.getColor(x+1, y) : img.getColor(x, y);
            ofColor up = (y > 0) ? img.getColor(x, y-1) : img.getColor(x, y);
            ofColor down = (y < height-1) ? img.getColor(x, y+1) : img.getColor(x, y);
            
            float dx = abs(left.r - right.r) + abs(left.g - right.g) + abs(left.b - right.b);
            float dy = abs(up.r - down.r) + abs(up.g - down.g) + abs(up.b - down.b);
            
            energy[y][x] = dx + dy;
        }
    }
    return energy;
}

// 수직 심 찾기
vector<int> ofxSeamCarving::findVerticalSeam(const vector<vector<float>>& energy) {
    int height = energy.size();
    int width = energy[0].size();
    
    vector<vector<float>> dp = energy;
    vector<vector<int>> backtrack(height, vector<int>(width));
    
    // DP를 사용하여 최소 에너지 경로 계산
    for (int y = 1; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float minEnergy = dp[y-1][x];
            int minX = x;
            
            if (x > 0 && dp[y-1][x-1] < minEnergy) {
                minEnergy = dp[y-1][x-1];
                minX = x-1;
            }
            if (x < width-1 && dp[y-1][x+1] < minEnergy) {
                minEnergy = dp[y-1][x+1];
                minX = x+1;
            }
            
            dp[y][x] += minEnergy;
            backtrack[y][x] = minX;
        }
    }
    
    // 최소 에너지 경로 추적
    vector<int> seam(height);
    int minX = 0;
    float minEnergy = dp[height-1][0];
    
    for (int x = 1; x < width; x++) {
        if (dp[height-1][x] < minEnergy) {
            minEnergy = dp[height-1][x];
            minX = x;
        }
    }
    
    seam[height-1] = minX;
    for (int y = height-2; y >= 0; y--) {
        seam[y] = backtrack[y+1][seam[y+1]];
    }
    
    return seam;
}

// 수직 심 제거
void ofxSeamCarving::removeVerticalSeam(ofImage& img, const vector<int>& seam) {
    int width = img.getWidth();
    int height = img.getHeight();
    
    ofImage newImg;
    newImg.allocate(width-1, height, OF_IMAGE_COLOR);
    
    for (int y = 0; y < height; y++) {
        int seamX = seam[y];
        for (int x = 0; x < width-1; x++) {
            if (x < seamX) {
                newImg.setColor(x, y, img.getColor(x, y));
            } else {
                newImg.setColor(x, y, img.getColor(x+1, y));
            }
        }
    }
    
    img = newImg;
}

// Seam Carving으로 이미지 크기 조절
void ofxSeamCarving::resizeWithSeamCarving(ofImage& img, int targetWidth, int targetHeight, bool animated) {
    if (animated) {
        // 애니메이션 모드는 update()에서 처리됨
        this->targetWidth = targetWidth;
        this->targetHeight = targetHeight;
        return;
    }
    
    // 너비 조절
    while (img.getWidth() > targetWidth) {
        vector<vector<float>> energy = calculateEnergy(img);
        vector<int> seam = findVerticalSeam(energy);
        removeVerticalSeam(img, seam);
    }
    
    // 높이 조절 (회전 방식 사용)
    img.rotate90(1);
    while (img.getWidth() > targetHeight) {
        vector<vector<float>> energy = calculateEnergy(img);
        vector<int> seam = findVerticalSeam(energy);
        removeVerticalSeam(img, seam);
    }
    img.rotate90(-1);
}