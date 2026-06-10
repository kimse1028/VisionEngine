#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>

// 나중에 우리가 만들 '이미지 입력 노드(Input Node)'의 역할을 할 함수야.
std::unique_ptr<cv::Mat> loadImageNode(const std::string& filePath) {
    // 1. 힙 메모리에 cv::Mat 객체를 생성하고 동시에 이미지를 로드해.
    auto imgPtr = std::make_unique<cv::Mat>(cv::imread(filePath));

    // 2. 이미지가 잘 불러와졌는지 방어 코드를 작성해 주는 게 좋아.
    // 스마트 포인터 내부의 멤버에 접근할 때는 화살표(->) 연산자를 사용해.
    if (imgPtr->empty()) {
        std::cout << "이미지를 불러올 수 없어. 파일 이름과 경로를 확인해 줘!" << std::endl;
    }
    else {
        std::cout << "이미지 로드 성공! 크기: " << imgPtr->cols << " x " << imgPtr->rows << std::endl;
    }

    // 3. 이미지 버퍼의 '소유권'을 반환해. (데이터 복사 발생 X)
    return imgPtr;
}

int main() {
    std::cout << "Vision Engine 테스트 시작..." << std::endl;

    // A. 파이프라인 시작: 이미지 로드 노드 실행
    // sourceImage라는 유일한 주인이 test.jpg 메모리를 독점 관리하게 돼.
    std::unique_ptr<cv::Mat> sourceImage = loadImageNode("test.bmp");

    // B. 데이터 확인 및 시각화
    if (!sourceImage->empty()) {
        // 스마트 포인터가 가리키는 '실제 데이터'를 꺼낼 때는 별표(*)를 붙여. (역참조 연산자)
        cv::imshow("Vision Node Output", *sourceImage);

        // 사용자가 아무 키나 누를 때까지 창을 띄워두고 대기 (밀리초 단위, 0은 무한 대기)
        cv::waitKey(0);
    }

    // C. main 함수가 끝나면 sourceImage의 수명이 다하면서, 
    // unique_ptr이 알아서 이미지 메모리를 깔끔하게 청소해 줘. 메모리 누수 걱정 끝!
    return 0;
}