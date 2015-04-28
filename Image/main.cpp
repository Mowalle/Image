#include "DepthMap.hpp"
#include "ColorImage.hpp"

int main(int argc, char** argv)
{

    //ColorImage<float, ColorSpace::RGB> fImage("Test.jpeg");
    //ColorImage<float, ColorSpace::LAB> hsvImage;

    //const clock_t startTime = clock();

    //fImage.convertColorSpace(&hsvImage);

    //std::clog << float(clock() - startTime) / CLOCKS_PER_SEC << std::endl;

    //hsvImage.write("TestConvert.png");
    //fImage.write("TestUnconverted.png");

    //DepthMap dmp;

    //dmp.read("Bla.dat");

    //ColorImage<float, ColorSpace::GRAY> grayImg = convertDepthToGray(dmp);

    //grayImg.write("DepthGray.png");

    //std::cout << "\n----------"
    //             "\nend main()"
    //             "\n----------" 
    //          << std::endl;


    /*auto dMap = DepthMap{};
	dMap.setDepthMin(0.0f);
	dMap.setDepthMax(5.0f);

    dMap.read("D:\\rmorawe\\Personal Documents\\HiWi\\Depth Image Example Data\\depthMap_0750.dmp");
	dMap.write("C:\\Users\\rmorawe\\Desktop\\Test.dat");*/

    auto cImg = ColorImage<unsigned char, ColorSpace::RGBA>{};
    cImg.read("Test.png");
    cImg.write("TestResult.png");

    auto cImg2 = ColorImage<unsigned char, ColorSpace::RGB>{};
    cImg.convertColorSpace(&cImg2);

    return 0;
}