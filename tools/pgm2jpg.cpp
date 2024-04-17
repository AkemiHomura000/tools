#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
    // 读取 PGM 图像
    cv::String pgmPath ="/home/lp1/qt_test/nav.pgm";
    cv::Mat pgmImage = cv::imread(pgmPath, cv::IMREAD_GRAYSCALE);
    if (pgmImage.empty()) {
        std::cerr << "Failed to read input PGM image: " << pgmPath << std::endl;
        return 1;
    }
    // 将 PGM 图像转换为 JPG 格式
    cv::String jpgPath = "/home/lp1/qt_test/nav.jpg";
    bool success = cv::imwrite(jpgPath, pgmImage);
    if (!success) {
        std::cerr << "Failed to write output JPG image: " << jpgPath << std::endl;
        return 1;
    }
    std::cout << "Conversion from PGM to JPG completed successfully!" << std::endl;
    return 0;
}
