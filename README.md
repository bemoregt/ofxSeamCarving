# ofxSeamCarving

openFrameworks를 위한 콘텐츠 인식 이미지 리사이징(Seam Carving) 애드온입니다.

## 개요

ofxSeamCarving은 Seam Carving 알고리즘을 사용하여 이미지의 중요한 컨텐츠를 보존하면서 크기를 조절할 수 있는 openFrameworks 애드온입니다. 이 알고리즘은 이미지에서 시각적으로 덜 중요한 픽셀들을 식별하고 제거하여 이미지의 비율과 중요한 특성을 유지하면서 크기를 변경합니다.

Seam Carving은 일반적인 스케일링과 달리 중요한 이미지 요소를 왜곡하지 않고 크기를 조절할 수 있어 다양한 시각적 응용 프로그램에서 유용하게 사용됩니다.

## 기능

- 콘텐츠 인식 이미지 리사이징
- 너비 또는 높이 방향 개별 조절 지원
- 애니메이션 효과를 통한 단계적 리사이징
- 에너지 맵 시각화
- 리사이징 완료 이벤트 콜백

## 설치 방법

ofxSeamCarving을 사용하기 위해 다음 단계를 따르세요:

1. 이 저장소를 openFrameworks의 addons 폴더에 클론합니다:
```
cd openFrameworks/addons
git clone https://github.com/bemoregt/ofxSeamCarving.git
```

2. Project Generator를 사용하거나 수동으로 프로젝트에 애드온을 추가합니다.

## 사용 방법

```cpp
// ofApp.h에서
#include "ofxSeamCarving.h"

class ofApp : public ofBaseApp {
public:
    // ...
    ofxSeamCarving seamCarver;
    // ...
};

// ofApp.cpp에서
void ofApp::setup() {
    seamCarver.setup();
    seamCarver.loadImage("image.jpg");
}

void ofApp::update() {
    seamCarver.update();  // 애니메이션 모드에서 필요
}

void ofApp::draw() {
    // 결과 이미지 그리기
    seamCarver.getResultImage().draw(0, 0);
    
    // 또는 에너지 맵 시각화
    seamCarver.drawEnergyMap(0, 0, 300, 200);
}

// 이미지 리사이징
void ofApp::keyPressed(int key) {
    if(key == 'r') {
        // 즉시 리사이징
        seamCarver.resizeTo(400, 300);
    }
    else if(key == 'a') {
        // 애니메이션 리사이징
        seamCarver.resizeWithCallback(400, 300, true);
    }
}
```

## 주요 메서드

- `setup()` - 애드온 초기화
- `loadImage(string path)` - 이미지 파일 로드
- `setImage(ofImage& img)` - ofImage 객체 설정
- `update()` - 애니메이션 모드 업데이트 (ofApp::update()에서 호출)
- `resizeTo(int width, int height)` - 지정 크기로 즉시 리사이징
- `resizeToWidth(int width)` - 지정 너비로만 리사이징
- `resizeToHeight(int height)` - 지정 높이로만 리사이징
- `resizeWithCallback(int width, int height, bool animated)` - 콜백과 애니메이션 옵션 리사이징
- `getResultImage()` - 결과 이미지 가져오기
- `saveResultImage(string path)` - 결과 이미지 저장
- `drawEnergyMap(float x, float y, float w, float h)` - 에너지 맵 시각화
- `drawWithSeam(float x, float y, float w, float h)` - 현재 심이 표시된 이미지 그리기

## 예제

`/example` 폴더에 포함된 예제 프로젝트에서 다음과 같은 기능을 확인할 수 있습니다:

1. 드래그 앤 드롭으로 이미지 로드
2. 마우스 드래그로 인터랙티브 리사이징
3. 키보드 단축키로 에너지 맵 토글
4. 결과 이미지 저장

## 알고리즘 설명

Seam Carving은 다음 단계로 작동합니다:

1. **에너지 계산**: 각 픽셀의 중요도를 나타내는 에너지 값 계산
2. **심 찾기**: 동적 프로그래밍을 사용하여 최소 에너지 경로(심) 찾기
3. **심 제거**: 찾은 심을 제거하고 이미지 재구성

이 과정을 원하는 크기에 도달할 때까지 반복합니다.

## 성능 고려사항

- 대용량 이미지의 경우 처리 시간이 오래 걸릴 수 있습니다.
- 애니메이션 모드는 프레임 속도에 영향을 줄 수 있으므로 필요에 따라 사용하세요.
- 최소 이미지 크기(100x100 픽셀)를 유지하는 것이 좋습니다.

## 향후 개선 가능성

- 다양한 에너지 함수 구현 (엔트로피 기반, 소벨 필터 등)
- 객체 인식 및 보존 영역 마스킹
- 멀티스레딩 처리 최적화
- 이미지 확대(심 삽입) 기능 추가

## 라이센스

MIT License

## 출처

이 애드온은 Shai Avidan과 Ariel Shamir의 2007년 논문 "Seam Carving for Content-Aware Image Resizing"에서 제안된 알고리즘을 기반으로 합니다.