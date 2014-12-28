#include "ColorImage.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

int main( int argc, char** argv )
{
	std::cout << "Testing image class..." << std::endl;

	ColorImage<unsigned char, ColorSpace::CS_RGBA> img( "Test.png" );
	ColorImage<float, ColorSpace::CS_HSV> img2( 64, 32 );
	ColorImage<float, ColorSpace::CS_GRAY> img3( 64, 32 );


	//img.copy( &img2 );
	img.convert( &img2 );
	img2.convert( &img3 );


	img.write( "NewImage1.png" );
	img2.write( "NewImage2.png" );
	img3.write( "NewImage3.png" );
	std::cout << "\n--------------------" << std::endl;
	std::cout << "End of program." << std::endl;
	
	return 0;
}