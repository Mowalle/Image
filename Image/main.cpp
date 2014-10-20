#include "ColorImage.h"

#include <iostream>

int main()
{
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