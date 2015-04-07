#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

//#include "ColorImage.h"
#include "DepthMap.h"
#include "ColorImage.h"

#include<iostream>
#include<time.h>

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

    ColorImage<float, ColorSpace::CS_GRAY> grayImg;

    DepthMap dMap;
	dMap.setDepthMin(0.0f);
	dMap.setDepthMax(5.0f);

    dMap.read("C:\\Users\\rmorawe\\Personal Documents\\Depth Image Example Data\\depthMap_0750.dmp");
	dMap.write("C:\\Users\\rmorawe\\Desktop\\Test.dat");

    grayImg = convertDepthToGray(dMap);
    grayImg.write("TestGrayNew.png");

    return 0;
}