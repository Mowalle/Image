#include "ColorImage.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

int main( int argc, char** argv )
{
	std::cout << "Testing image class..." << std::endl;

	ColorImage<float, ColorSpace::CS_ARGB> img;

	std::string fileName = "Test.png";

	if ( img.read( fileName ) )
	{
		std::cout << "File was read successfully!" << std::endl;
		std::cout << "Image size: " << img.getWidth() << " x " << img.getHeight() << std::endl;
		std::cout << "Number of Channels: " << img.getNumberOfChannels() << std::endl;

		std::cout << (int) img.getData()[940] << "," << (int) img.getData()[941] << "," << (int) img.getData()[942] << "," << (int) img.getData()[943] << std::endl;

		img.write( "NewImage.png" );

	} 
	else
	{
		std::cout << "File wasn't read!" << std::endl;
	}

	std::cout << "\n--------------------" << std::endl;
	std::cout << "End of program." << std::endl;
	
	return 0;
}