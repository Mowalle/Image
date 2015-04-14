#include "DepthMap.h"
#include "ColorImage.h"

int main(int argc, char** argv)
{

    //ColorImage<float, ColorSpace::CS_RGB> fImage("Test.jpeg");
    //ColorImage<float, ColorSpace::CS_LAB> hsvImage;

    //const clock_t startTime = clock();

    //fImage.convertColorSpace(&hsvImage);

    //std::clog << float(clock() - startTime) / CLOCKS_PER_SEC << std::endl;

    //hsvImage.write("TestConvert.png");
    //fImage.write("TestUnconverted.png");

    //DepthMap dmp;

    //dmp.read("Bla.dat");

    //ColorImage<float, ColorSpace::CS_GRAY> grayImg = convertDepthToGray(dmp);

    //grayImg.write("DepthGray.png");

    //std::cout << "\n----------"
    //             "\nend main()"
    //             "\n----------" 
    //          << std::endl;


    DepthMap dMap;
	dMap.setDepthMin(0.0f);
	dMap.setDepthMax(5.0f);

    dMap.read("D:\\rmorawe\\Personal Documents\\HiWi\\Depth Image Example Data\\depthMap_0750.dmp");
	dMap.write("C:\\Users\\rmorawe\\Desktop\\Test.dat");


    auto grayImg = convertDepthToGray(dMap);
    grayImg.write("TestGrayNew.png");

    return 0;
}