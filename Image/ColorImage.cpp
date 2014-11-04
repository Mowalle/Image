/**
*	This file must be excluded from build!
**/

#include <iostream>
#include <fstream>

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::read( const std::string &fileName )
{
	// Create dummy file and check if it was a success before trying to 
	// read image.

	// Opening dummy file to check if it exists at all.
	// Replaced the old C-directives (may be slower).
	std::ifstream dummy;
	dummy.open( fileName );

	if ( !dummy.is_open() )
	{
		std::cout << "ERROR: Couldn't open or find file " << fileName << ".\n";
		return false;
	}

	dummy.close();

	bool success;
	
	std::cout << "Trying to open OpenCV-file..." << std::endl;
	// Use OpenCV to read image, replacing readCV(fileName, cf)
	success = readCV( fileName );
	
	return success;
}

/** ------------------------------------
*					private
*	------------------------------------
**/

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::readCV( const std::string &fileName )
{
	// Load image
	cv::Mat cvImage = cv::imread( fileName, cv::IMREAD_UNCHANGED);

	// If image wasn't loaded (wrong format, etc.), exit
	if ( !cvImage.data )
	{
		std::cout << "ERROR: OpenCV failed to load file " << fileName << ".\n";
		return false;
	}

	// TODO: If for example CS_RGB is chosen and image has 4 channels, ignore the alpha.
	if ( cvImage.channels() != m_numChan )
	{
		std::cout << "ERROR: Image doesn't have compatible number of channels!" << std::endl;
		std::cout << "(Expected " << m_numChan << "channels, but loaded image has " << cvImage.channels() << " channels.)" << std::endl; 
		return false;
	}

	m_width = cvImage.size().width;
	m_height = cvImage.size().height;

	// Allocate Memory if necessary
	bool reallocmem = ( m_data == nullptr ); // is a new image?
	
	// If needed memory exceeds already allocated memory
	if ( ( cvImage.channels() * cvImage.size().width * cvImage.size().height > m_memAllocated ) )
	{
		reallocateMemory();
	}

	// TODO: Templated: Parse Image data into array, depending on Type.
	copyDataFromCV( cvImage );

	// openCV cv::Mat handles memory autamtically -> no destructor call needed.
	return true;
}


template <typename Type, ColorSpace cs>
void ColorImage<Type, cs>::writeCV( const std::string &fileName ) const
{
	cv::Mat cvImage;
	Type* data = nullptr;
	bool writeReady = false;
}


template <typename Type, ColorSpace cs>
void ColorImage<Type, cs>::reallocateMemory()
{
	delete [] m_data;
	m_data = nullptr;

	setRGBAOffsets();

	m_memAllocated = m_numChan * m_width * m_height;
	std::cout << "Allocated " << m_memAllocated << std::endl;
	m_data = new Type[m_memAllocated];
}

template <typename Type, ColorSpace cs>
void ColorImage<Type, cs>::setRGBAOffsets()
{
	switch ( cs )
	{
	case ColorSpace::CS_GRAY:
		m_offsetR = m_offsetG = m_offsetB = m_offsetA = 0;
		break;
	case ColorSpace::CS_RGB:
	case ColorSpace::CS_HSV:
	case ColorSpace::CS_LAB:
		m_offsetR = 0;
		m_offsetG = 1;
		m_offsetB = 2;
		break;
	case ColorSpace::CS_BGR:
		m_offsetR = 2;
		m_offsetG = 1;
		m_offsetB = 0;
		break;
	case ColorSpace::CS_RGBA:
		m_offsetR = 0;
		m_offsetG = 1;
		m_offsetB = 2;
		m_offsetA = 3;
		break;
	case ColorSpace::CS_BGRA:
		m_offsetR = 2;
		m_offsetG = 1;
		m_offsetB = 0;
		m_offsetA = 3;
		break;
	default:
		m_offsetR = m_offsetG = m_offsetB = m_offsetA = 0;
		break;
	}
}

template <>
void ColorImage<float, ColorSpace::CS_GRAY>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<float, ColorSpace::CS_GRAY>( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_RGB>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<float, ColorSpace::CS_RGB>( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_HSV>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<float, ColorSpace::CS_HSV>( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_LAB>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<float, ColorSpace::CS_LAB>( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_BGR>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<float, ColorSpace::CS_BGR>( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_RGBA>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<float, ColorSpace::CS_RGBA>( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_BGRA>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<float, ColorSpace::CS_BGRA>( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_ARGB>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<float, ColorSpace::CS_ARGB>( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_GRAY>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<unsigned char, ColorSpace::CS_GRAY>( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_RGB>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<unsigned char, ColorSpace::CS_RGB>( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_HSV>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<unsigned char, ColorSpace::CS_HSV>( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_LAB>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<unsigned char, ColorSpace::CS_LAB>( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_BGR>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<unsigned char, ColorSpace::CS_BGR>( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_RGBA>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<unsigned char, ColorSpace::CS_RGBA>( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_BGRA>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<unsigned char, ColorSpace::CS_BGRA>( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_ARGB>::copyDataFromCV( const cv::Mat &cvImage )
{
	::copyDataFromCV<unsigned char, ColorSpace::CS_ARGB>( cvImage, this, 1.0f );
}

/** ------------------------------------
*					global
*	------------------------------------
**/

template <typename Type, ColorSpace cs>
void copyDataFromCV( const cv::Mat &cvImage, ColorImage<Type, cs> *clrImage , float scale )
{
	int numColumns = cvImage.size().width * cvImage.channels();
	int step = cvImage.step;

	uchar* data = cvImage.data;
	int pixelCount = 0;

	if ( cvImage.channels() == 1 )
	{
		for ( int i = 0; i < clrImage->getHeight(); ++i )
		{
			for ( int j = 0; j < numColumns; ++j )
			{
				clrImage->getData()[pixelCount] = data[j];
				pixelCount++;
			}
			data += step; // next line
		}
	}
	else if ( cvImage.channels() == 3 )
	{
		for ( int i = 0; i < clrImage->getHeight(); ++i )
		{
			for ( int j = 0; j < numColumns; ++j )
			{
				clrImage->getData()[clrImage->b( pixelCount )] = data[j] * scale;
				clrImage->getData()[clrImage->g( pixelCount )] = data[j + 1] * scale;
				clrImage->getData()[clrImage->r( pixelCount )] = data[j + 2] * scale;
				pixelCount++;
			}
			data += step; // next line
		}
	}
}