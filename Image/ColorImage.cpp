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

template <typename Type, ColorSpace cs>
void ColorImage<Type, cs>::write( const std::string &fileName ) const
{
	writeCV( fileName );
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
	if ( cvImage.empty() )
	{
		std::cout << "ERROR: OpenCV failed to load file " << fileName << ".\n";
		return false;
	}

	// TODO: If for example CS_RGB is chosen and image has 4 channels, ignore the alpha.
	if ( cvImage.channels() != m_numChan )
	{
		std::cout << "ERROR: Image doesn't have compatible number of channels!" << std::endl;
		std::cout << "(Expected " << m_numChan << " channels, but loaded image has " << cvImage.channels() << " channels.)" << std::endl; 
		return false;
	}

	m_width = cvImage.size().width;
	m_height = cvImage.size().height;

	// If needed memory exceeds already allocated memory
	if ( ( cvImage.channels() * cvImage.size().width * cvImage.size().height > m_memAllocated ) )
	{
		reallocateMemory();
	}

	copyDataFromCV( cvImage );
	// openCV cv::Mat handles memory autamtically -> no destructor call needed.
	return true;
}

template <>
void ColorImage<float, ColorSpace::CS_GRAY>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<float, ColorSpace::CS_GRAY>::copyDataFromCVHelper( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_RGB>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<float, ColorSpace::CS_RGB>::copyDataFromCVHelper( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_HSV>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<float, ColorSpace::CS_HSV>::copyDataFromCVHelper( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_LAB>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<float, ColorSpace::CS_LAB>::copyDataFromCVHelper( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_BGR>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<float, ColorSpace::CS_BGR>::copyDataFromCVHelper( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_RGBA>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<float, ColorSpace::CS_RGBA>::copyDataFromCVHelper( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_BGRA>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<float, ColorSpace::CS_BGRA>::copyDataFromCVHelper( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<float, ColorSpace::CS_ARGB>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<float, ColorSpace::CS_ARGB>::copyDataFromCVHelper( cvImage, this, (1/255.0f) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_GRAY>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<unsigned char, ColorSpace::CS_GRAY>::copyDataFromCVHelper( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_RGB>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<unsigned char, ColorSpace::CS_RGB>::copyDataFromCVHelper( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_HSV>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<unsigned char, ColorSpace::CS_HSV>::copyDataFromCVHelper( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_LAB>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<unsigned char, ColorSpace::CS_LAB>::copyDataFromCVHelper( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_BGR>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<unsigned char, ColorSpace::CS_BGR>::copyDataFromCVHelper( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_RGBA>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<unsigned char, ColorSpace::CS_RGBA>::copyDataFromCVHelper( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_BGRA>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<unsigned char, ColorSpace::CS_BGRA>::copyDataFromCVHelper( cvImage, this, 1.0f );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_ARGB>::copyDataFromCV( const cv::Mat &cvImage )
{
	ColorImage<unsigned char, ColorSpace::CS_ARGB>::copyDataFromCVHelper( cvImage, this, 1.0f );
}

template <typename Type, ColorSpace cs>
void ColorImage<Type, cs>::copyDataFromCVHelper( const cv::Mat &cvImage, ColorImage<Type, cs> *clrImage , float scale )
{
	int numColumns = cvImage.size().width * cvImage.channels();
	int step = cvImage.step;

	unsigned char* data = cvImage.data;
	int pixelCount = 0;

	if ( cvImage.channels() == 1 )
	{
		for ( int i = 0; i < m_height; ++i )
		{
			for ( int j = 0; j < numColumns; ++j )
			{
				m_data[pixelCount] = data[j];
				pixelCount++;
			}
			data += step; // next line
		}
	}
	else if ( cvImage.channels() == 3 )
	{
		for ( int i = 0; i < m_height; ++i )
		{
			for ( int j = 0; j < numColumns; j += cvImage.channels() )
			{
				m_data[b( pixelCount )] = data[j] * scale;
				m_data[g( pixelCount )] = data[j + 1] * scale;
				m_data[r( pixelCount )] = data[j + 2] * scale;
				pixelCount++;
			}
			data += step; // next line
		}
	}
	else if ( cvImage.channels() == 4 )
	{
		for ( int i = 0; i < m_height; ++i )
		{
			for ( int j = 0; j < numColumns; j += cvImage.channels() )
			{
				m_data[b( pixelCount )] = data[j] * scale;
				m_data[g( pixelCount )] = data[j + 1] * scale;
				m_data[r( pixelCount )] = data[j + 2] * scale;
				m_data[a( pixelCount )] = data[j + 3] * scale;
				pixelCount++;
			}
			data += step; // next line
		}
	}
}

template <>
void ColorImage<float, ColorSpace::CS_GRAY>::writeCV( const std::string& fileName ) const
{
	float* tmp = new float[m_memAllocated];

	ColorImage<float, ColorSpace::CS_GRAY>::writeCVHelper( tmp, 255.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC1, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<float, ColorSpace::CS_RGB>::writeCV( const std::string& fileName ) const
{
	float* tmp = new float[m_memAllocated];

	ColorImage<float, ColorSpace::CS_RGB>::writeCVHelper( tmp, 255.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC3, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<float, ColorSpace::CS_HSV>::writeCV( const std::string& fileName ) const
{
	float* tmp = new float[m_memAllocated];

	ColorImage<float, ColorSpace::CS_HSV>::writeCVHelper( tmp, 255.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC3, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<float, ColorSpace::CS_LAB>::writeCV( const std::string& fileName ) const
{
	float* tmp = new float[m_memAllocated];

	ColorImage<float, ColorSpace::CS_LAB>::writeCVHelper( tmp, 255.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC3, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<float, ColorSpace::CS_BGR>::writeCV( const std::string& fileName ) const
{
	float* tmp = new float[m_memAllocated];

	ColorImage<float, ColorSpace::CS_BGR>::writeCVHelper( tmp, 255.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC3, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<float, ColorSpace::CS_RGBA>::writeCV( const std::string& fileName ) const
{
	float* tmp = new float[m_memAllocated];

	ColorImage<float, ColorSpace::CS_RGBA>::writeCVHelper( tmp, 255.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC4, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<float, ColorSpace::CS_BGRA>::writeCV( const std::string& fileName ) const
{
	float* tmp = new float[m_memAllocated];

	ColorImage<float, ColorSpace::CS_BGRA>::writeCVHelper( tmp, 255.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC4, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<float, ColorSpace::CS_ARGB>::writeCV( const std::string& fileName ) const
{
	float* tmp = new float[m_memAllocated];

	ColorImage<float, ColorSpace::CS_ARGB>::writeCVHelper( tmp, 255.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC4, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_GRAY>::writeCV( const std::string& fileName ) const
{
	unsigned char* tmp = new unsigned char[m_memAllocated];

	ColorImage<unsigned char, ColorSpace::CS_GRAY>::writeCVHelper( tmp, 1.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC1, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_RGB>::writeCV( const std::string& fileName ) const
{
	unsigned char* tmp = new unsigned char[m_memAllocated];

	ColorImage<unsigned char, ColorSpace::CS_RGB>::writeCVHelper( tmp, 1.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC3, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_HSV>::writeCV( const std::string& fileName ) const
{
	unsigned char* tmp = new unsigned char[m_memAllocated];

	ColorImage<unsigned char, ColorSpace::CS_HSV>::writeCVHelper( tmp, 1.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC3, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_LAB>::writeCV( const std::string& fileName ) const
{
	unsigned char* tmp = new unsigned char[m_memAllocated];

	ColorImage<unsigned char, ColorSpace::CS_LAB>::writeCVHelper( tmp, 1.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC3, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_BGR>::writeCV( const std::string& fileName ) const
{
	unsigned char* tmp = new unsigned char[m_memAllocated];

	ColorImage<unsigned char, ColorSpace::CS_BGR>::writeCVHelper( tmp, 1.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC3, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_RGBA>::writeCV( const std::string& fileName ) const
{
	unsigned char* tmp = new unsigned char[m_memAllocated];

	ColorImage<unsigned char, ColorSpace::CS_RGBA>::writeCVHelper( tmp, 1.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC4, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_BGRA>::writeCV( const std::string& fileName ) const
{
	unsigned char* tmp = new unsigned char[m_memAllocated];

	ColorImage<unsigned char, ColorSpace::CS_BGRA>::writeCVHelper( tmp, 1.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC4, tmp ) );

	delete [] tmp;
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_ARGB>::writeCV( const std::string& fileName ) const
{
	unsigned char* tmp = new unsigned char[m_memAllocated];

	ColorImage<unsigned char, ColorSpace::CS_ARGB>::writeCVHelper( tmp, 1.0f );

	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC4, tmp ) );

	delete [] tmp;
}

template <typename Type, ColorSpace cs>
void ColorImage<Type, cs>::writeCVHelper( Type* trgData , const float scale ) const
{
	// TODO: Account for different ColorSpace (XYZ/XYZA -> BGR/BGRA)
	if ( m_numChan == 1 )
	{
		for ( int i = 0; i < m_memAllocated; ++i )
		{
			trgData[i] = m_data[i] * scale;
		}
	} 
	else if ( m_numChan == 3 )
	{
		for ( int i = 0; i < m_memAllocated; i += m_numChan )
		{
			trgData[i] = m_data[i + m_offsetB] * scale;
			trgData[i + 1] = m_data[i + m_offsetG] * scale;
			trgData[i + 2] = m_data[i + m_offsetR] * scale;
		}
	} 
	else if (m_numChan == 4) 
	{
		for ( int i = 0; i < m_memAllocated; i += m_numChan )
		{
			trgData[i] = m_data[i + m_offsetB] * scale;
			trgData[i + 1] = m_data[i + m_offsetG] * scale;
			trgData[i + 2] = m_data[i + m_offsetR] * scale;
			trgData[i + 3] = m_data[i + m_offsetA] * scale;
		}
	}
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
	case ColorSpace::CS_ARGB:
		m_offsetR = 1;
		m_offsetG = 2;
		m_offsetB = 3;
		m_offsetA = 0;
		break;
	default:
		m_offsetR = m_offsetG = m_offsetB = m_offsetA = 0;
		break;
	}
}
