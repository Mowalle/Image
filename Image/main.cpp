#include "ColorImage.h"

#include <iostream>

int main( int argc, char** argv )
{
	std::cout << "Testing image class..." << std::endl;

	ColorImage<unsigned char, ColorSpace::CS_RGBA> img;
	
	std::string fileName = "Test.png";

	if ( img.read( fileName ) )
	{
		std::cout << "File was read successfully!" << std::endl;
		std::cout << "Image size: " << img.getWidth() << " x " << img.getHeight() << std::endl;
	}
	else
	{
		std::cout << "File wasn't read!" << std::endl;
	}

	std::cout << "\n--------------------" << std::endl;
	std::cout << "End of program." << std::endl;
	
	return 0;
}