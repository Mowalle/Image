#include "ColorImage.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <vector>
#include <stdio.h>

int main(int argc, char** argv)
{
    //std::cout << "Testing image class..." << std::endl;

    //ColorImage<unsigned char, ColorSpace::CS_RGBA> img("Test.png");
    //ColorImage<float, ColorSpace::CS_HSV> img2(64, 32);
    //ColorImage<float, ColorSpace::CS_GRAY> img3(64, 32);


    ////img.copy(&img2);
    //img.convert(&img2);
    //img2.convert(&img3);

    //img.write("NewImage1.png");
    //img2.write("NewImage2.png");
    //img3.write("NewImage3.png");
    //std::cout << "\n--------------------" << std::endl;
    //std::cout << "End of program." << std::endl;

    ColorImage<float, ColorSpace::CS_HSV > c1("TestHsv.jpeg");
    std::cout << c1.getWidth() << ", " << c1.getHeight() << std::endl;

    std::cout << "----------" << std::endl;
    for (int j = 0; j < c1.getHeight(); ++j) 
    {
        for (int i = 0; i < c1.getWidth(); ++i) 
        {
            std::cout << c1.getData()[c1.H(i, j)] << ", " <<
                         c1.getData()[c1.S(i, j)] << ", " <<
                         c1.getData()[c1.V(i, j)] << std::endl;
        }
    }

    c1.write("TestHsvResult.png");

    ColorImage<float, ColorSpace::CS_RGBA> c2(3, 2);
    ColorImage<unsigned char, ColorSpace::CS_RGBA> c3(3, 2);

    c1.convertColorSpace(&c2);
    c2.convertType(&c3);

    c3.write("TestHsvResultConvert.png");

    return 0;
}