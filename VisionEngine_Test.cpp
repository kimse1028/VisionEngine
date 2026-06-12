#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>

// 모든 영상처리 노드가 지켜야 할 공통 규격 (인터페이스)
class BaseNode {
public:
    // 가상 소멸자: 부모를 통해 자식을 삭제할 때, 자식의 메모리까지 깔끔하게 지우기 위한 필수 안전장치야.
    virtual ~BaseNode() = default;

    // 순수 가상 함수(= 0): "나는 껍데기일 뿐이니, 나를 상속받는 자식들은 무조건 이 함수를 직접 완성해라"는 강제 명령이야.
    // 입력을 shared_ptr로 받고, 출력도 shared_ptr로 뱉어내.
    virtual std::shared_ptr<cv::Mat> process(std::shared_ptr<cv::Mat> input) = 0;
};

// BaseNode를 상속(public) 받는 ThresholdNode 클래스
class ThresholdNode : public BaseNode {
public:
    // 부모가 강제한 process 함수를 여기서 직접 구현해.
    // override 키워드는 "부모의 함수를 내가 안전하게 덮어썼음"을 컴파일러에게 알려주는 확인 도장이야.
    std::shared_ptr<cv::Mat> process(std::shared_ptr<cv::Mat> input) override {
        // 1. 방어 코드: 들어온 포스트잇이 비어있거나, 창고에 사진이 없으면 그냥 빈 포스트잇을 돌려보내.
        if (!input || input->empty()) {
            return nullptr;
        }

        // 2. 결과물을 담을 '새로운 창고'를 하나 빌리고 빈 포스트잇(output)을 만들어.
        auto output = std::make_shared<cv::Mat>();

        // 3. OpenCV 이진화(Threshold) 작업 수행
        // *input(원본 창고)에서 사진을 꺼내서 이진화 처리를 한 뒤, *output(새 창고)에 넣어.
        // 128보다 어두우면 0(검정), 밝으면 255(흰색)로 만들어버리는 함수야.
        cv::threshold(*input, *output, 128, 255, cv::THRESH_BINARY);

        // 4. 이진화가 완료된 새 사진의 주소가 적힌 포스트잇을 다음 노드로 넘겨.
        return output;
    }
};

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