#include <iostream>
#include <memory>
#include <string>
#include <opencv2/opencv.hpp>

// 1. 공통 규격 (메뉴판)
class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual std::shared_ptr<cv::Mat> process(std::shared_ptr<cv::Mat> input) = 0;
};

// 2. 이미지 입력 노드 (파일에서 이미지를 가져옴)
class InputNode : public BaseNode {
private:
    std::string filePath; // 불러올 이미지 경로 저장

public:
    // 생성자: 노드가 만들어질 때 파일 경로를 미리 입력받아.
    InputNode(const std::string& path) {
        filePath = path;
    }

    // InputNode는 이전 노드에서 받는 input이 없으므로 무시하고 디스크에서 읽어와.
    std::shared_ptr<cv::Mat> process(std::shared_ptr<cv::Mat> input) override {
        auto output = std::make_shared<cv::Mat>(cv::imread(filePath));
        if (output->empty()) {
            std::cout << "에러: 이미지를 찾을 수 없어!" << std::endl;
            return nullptr; // 실패하면 빈 포스트잇 반환
        }
        return output;
    }
};

// 3. 이진화 노드 (이미지를 흑백으로 만듦)
class ThresholdNode : public BaseNode {
public:
    std::shared_ptr<cv::Mat> process(std::shared_ptr<cv::Mat> input) override {
        if (!input || input->empty()) return nullptr;

        auto output = std::make_shared<cv::Mat>(); 
        // 128을 기준으로 흑과 백으로 나눔
        cv::threshold(*input, *output, 128, 255, cv::THRESH_BINARY);

        return output;
    }
};

// 4. 메인 엔진 (GUI에서 선을 연결하는 역할을 대신함)
int main() {
    std::cout << "노드 기반 비전 엔진 파이프라인 가동..." << std::endl;

    // [Step A] 부품(노드) 생성
    // GUI에서 사용자가 화면에 'Input 상자'와 'Threshold 상자'를 끌어다 놓은 상황과 같아.
    auto node1 = std::make_shared<InputNode>("test.bmp");
    auto node2 = std::make_shared<ThresholdNode>();

    // [Step B] 파이프라인 연결 및 데이터 흐름 제어
    // currentBuffer는 노드와 노드 사이를 이동하며 사진을 전달하는 '공유 포스트잇'이야.
    std::shared_ptr<cv::Mat> currentBuffer = nullptr;

    // 1번 노드 가동: 디스크에서 사진을 가져와서 포스트잇에 담음
    currentBuffer = node1->process(currentBuffer);

    // 2번 노드 가동: 1번이 넘겨준 사진을 받아서 흑백으로 만들고 다시 포스트잇을 업데이트함
    // (이게 바로 GUI에서 마우스로 node1의 Output과 node2의 Input을 선으로 이은 것과 완벽히 같은 로직이야!)
    currentBuffer = node2->process(currentBuffer);

    // [Step C] 최종 결과물 시각화
    if (currentBuffer && !currentBuffer->empty()) {
        cv::namedWindow("Pipeline Result", cv::WINDOW_NORMAL);
        cv::imshow("Pipeline Result", *currentBuffer);
        cv::waitKey(0);
    }

    return 0;
}