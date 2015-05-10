#include "DepthMap.hpp"
#include "ColorImage.hpp"

#include <opencv2\imgproc\imgproc.hpp>

int main(int argc, char** argv)
{
    auto hsvF = ColorImage<float, ColorSpace::HSV>{};
    auto hsvUC = ColorImage<uc, ColorSpace::HSV>{};
    auto bgrF = ColorImage<float, ColorSpace::BGR>{};
    auto bgrUC = ColorImage<uc, ColorSpace::BGR>{};

    hsvF.read("Test.jpg");
    hsvF.convertColorSpace(&bgrF);
    hsvF.write("TestHSV_F.png");
    bgrF.write("TestBGR_F.png");
    
    hsvUC.read("Test.jpg");
    const unsigned char &h = hsvUC.getData()[3708];
    const unsigned char &s = hsvUC.getData()[3709];
    const unsigned char &v = hsvUC.getData()[3710];

    hsvUC.convertColorSpace(&bgrUC);
    hsvUC.write("TestHSV_UC.png");
    bgrUC.write("TestBGR_UC.png");

    // OpenCV for comparison
    auto test = cv::imread("Test.jpg", CV_LOAD_IMAGE_UNCHANGED);
    cv::imwrite("TestBGR_CV_UC.png", test);
    auto testHsv = cv::Mat{};
    cv::cvtColor(test, testHsv, CV_BGR2HSV);
    cv::imwrite("TestHSV_CV_UC.png", testHsv);

    return 0;
}