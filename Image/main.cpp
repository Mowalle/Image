#include "ColorImage.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include<iostream>
#include<vector>
#include<stdio.h>

int main(int argc, char** argv)
{
    ColorImage<float, ColorSpace::CS_BGRA> c1("Test.png");
    std::cout << c1.getWidth() << ", " << c1.getHeight() << std::endl;

    c1.setToBlack();
    c1.write("Test3Result.png");

    return 0;
}