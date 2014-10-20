#include "ColorImage.h"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


int main( int argc, char** argv )
{

	cv::Mat image;
	image = cv::imread( "Test.png", cv::IMREAD_COLOR );	// Read the file

	if ( !image.data )
	{
		std::cout << "Could not open or find the image!" << std::endl;
		return -1;
	}

	cv::namedWindow( "Display Window", cv::WINDOW_AUTOSIZE );	// Create Window to display image.
	cv::imshow( "Display Window", image );	// Show image inside it.
	cv::waitKey(0);

	std::cout << "Testing image class..." << std::endl;

	ColorImage<float, ColorSpace::CS_RGB> img;

	std::cout << "Image size: " << img.getWidth() << " x " << img.getHeight() << std::endl;

	std::string fileName = "Penguin.jpg";

	img.read( fileName );

	std::cout << sizeof( ColorSpace ) << std::endl;

	std::cout << "End of program.";
	
	// Keeps console open after execution.
	std::cout << "\n\nType any character to quit: ";
	char tmp;
	std::cin >> tmp;
	return 0;
}