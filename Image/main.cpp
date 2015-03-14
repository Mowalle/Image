#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "ColorImage.h"

#include<iostream>
#include<time.h>

int main(int argc, char** argv) 
{

    ColorImage<float, ColorSpace::CS_BGRA> fImage("Test.png");
	ColorImage<float, ColorSpace::CS_GRAY> hsvImage;

	const clock_t startTime = clock();

	//hsvImage.convertColorSpace(&fImage);
	fImage.convertColorSpace(&hsvImage);

	std::clog << float(clock() - startTime) / CLOCKS_PER_SEC << std::endl;

	hsvImage.write("TestConvert.png");
	fImage.write("TestUnconverted.png");

	std::cout << "\n----------"
		         "\nend main()"
				 "\n----------" 
			  << std::endl;

    return 0;
}