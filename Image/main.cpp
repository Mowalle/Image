#include "ColorImage.h"

#include <iostream>

int main( int argc, char** argv )
{
	std::cout << "Testing image class..." << std::endl;

	ColorImage<float, ColorSpace::CS_RGBA> img;

	std::string fileName = "Test.png";

	if ( img.read( fileName ) )
	{
		std::cout << "File was read successfully!" << std::endl;
		std::cout << "Image size: " << img.getWidth() << " x " << img.getHeight() << std::endl;
		std::cout << "Number of Channels: " << img.getNumberOfChannels() << std::endl;

		std::cout << "Pixel (11, 0): [" 
			<< img.getData()[44] * 255 << ", " << img.getData()[45] * 255 << ", "  
			<< img.getData()[46] * 255 << ", " << img.getData()[47] * 255 << "]"  << std::endl;

	}
	else
	{
		std::cout << "File wasn't read!" << std::endl;
	}

	std::cout << "\n--------------------" << std::endl;
	std::cout << "End of program." << std::endl;
	
	return 0;
}