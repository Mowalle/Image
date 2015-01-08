/**
*	This file must be excluded from build!
**/

#include <iostream>
#include <fstream>

template<typename T, ColorSpace C>
ColorImage<T, C>::ColorImage()
	: m_numChan( getNumberOfChannels( C ) )
{
	setRGBAOffsets();
}

template<typename T, ColorSpace C>
ColorImage<T, C>::ColorImage( int width, int height ) 
	: Image( width, height ), m_numChan( getNumberOfChannels( C ) ) 
{ 
	reallocateMemory(); 
}

template<typename T, ColorSpace C>
ColorImage<T, C>::ColorImage( const std::string& fileName )
	: m_numChan( getNumberOfChannels( C ) )
{
	read( fileName );
}

template<typename T, ColorSpace C>
ColorImage<T, C>::~ColorImage()
{
	std::cout << "ColorImage destructor" << std::endl;
}

template< typename T, ColorSpace C>
int ColorImage<T, C>::getNumberOfChannels( const ColorSpace colorsp )
{
	switch ( colorsp )
	{
	case ColorSpace::CS_GRAY:
		return 1;
		break;
	case ColorSpace::CS_RGB:
	case ColorSpace::CS_HSV:
	case ColorSpace::CS_LAB:
	case ColorSpace::CS_BGR:
		return 3;
		break;
	case ColorSpace::CS_BGRA:
	case ColorSpace::CS_RGBA:
	case ColorSpace::CS_ARGB:
		return 4;
		break;
	default:
		return 0;
		break;
	}
}

template <typename T, ColorSpace C>
bool ColorImage<T, C>::read( const std::string &fileName )
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

template <typename T, ColorSpace C>
void ColorImage<T, C>::write( const std::string &fileName ) const
{
	writeCV( fileName );
}

template <typename T, ColorSpace C>
void ColorImage<T, C>::copy( ColorImage* dst ) const
{
	// Check for dimensions
	if ( ( m_width == dst->m_width ) && ( m_height == dst->m_height ) && ( m_numChan == dst->m_numChan ) )
	{
		for ( int i = 0; i < m_memAllocated; ++i )
		{
			dst->m_data[i] = m_data[i];
		}
	}
	else // TODO: What if not compatible
	{

	}
}

template< typename T, ColorSpace C >
template< typename U, ColorSpace D >
void ColorImage< T, C >
::convert( ColorImage< U, D>* out ) const
{
	int outNumChan = out->getNumberOfChannels();
	int outMemAllocated = out->getMemAllocated();

	delete[] out->getData();

	U* data = new U[outMemAllocated];

	// Case 1: If both images have same number of channels
	if ( m_numChan == outNumChan )
	{
		if ( outNumChan == 1 ) // if grey iamge, copy value with specialized scale
		{
			for ( int i = 0; i < outMemAllocated; ++i )
			{
				data[i] = Image<T>::convertValue( m_data[i], U() );
			}
		}
		else // if 3 or 4 channels, copy to respective channel with specialized scale.
		{
			for ( int i = 0; i < outMemAllocated; i += outNumChan )
			{
				data[i + out->getOffsetB()] = Image<T>::convertValue( m_data[i + m_offsetB], U() );
				data[i + out->getOffsetG()] = Image<T>::convertValue( m_data[i + m_offsetG], U() );
				data[i + out->getOffsetR()] = Image<T>::convertValue( m_data[i + m_offsetR], U() );
				if ( outNumChan == 4 ) // if fourth channel (alpha) is present
				{
					data[i + out->getOffsetA()] = Image<T>::convertValue( m_data[i + m_offsetA], U() );
				}
			}
		}
	}
	// Case 2: input image has more channels
	else if ( m_numChan > outNumChan )
	{
		// input image shall be converted to grey image
		if ( ( m_numChan == 3 || m_numChan == 4 ) && outNumChan == 1 )
		{
			int pixelOffset = 0;
			// average color value
			float averageColor = 0;

			for ( int i = 0; i < outMemAllocated; ++i )
			{
				// Calculate average color value and assign it.
				// TODO: Other weights (luminosity etc.) instead of just average color.
				averageColor = (
					m_data[pixelOffset + m_offsetB] +
					m_data[pixelOffset + m_offsetG] +
					m_data[pixelOffset + m_offsetR] ) / 3;
				data[i] = Image<T>::convertValue( averageColor, U() );

				pixelOffset += m_numChan;
			}
		}
		// input image shall be converted to image without alpha
		else if ( m_numChan == 4 && outNumChan == 3 )
		{
			int pixelOffset = 0;
			for ( int i = 0; i < outMemAllocated; i += outNumChan )
			{
				data[i + out->getOffsetB()] = Image<T>::convertValue( m_data[pixelOffset + m_offsetB], U() );
				data[i + out->getOffsetG()] = Image<T>::convertValue( m_data[pixelOffset + m_offsetG], U() );
				data[i + out->getOffsetR()] = Image<T>::convertValue( m_data[pixelOffset + m_offsetR], U() );

				pixelOffset += m_numChan;
			}
		}
	}
	// Case 3 : input image has less channels
	else if ( m_numChan < outNumChan )
	{
		// Grey image shall be converted into image with 3/4 channels.
		if ( m_numChan == 1 && ( outNumChan == 3 || outNumChan == 4 ) )
		{
			int pixelOffset = 0;
			for ( int i = 0; i < outMemAllocated; i += outNumChan )
			{
				data[i + out->getOffsetB()] = Image<T>::convertValue( m_data[pixelOffset], U() );
				data[i + out->getOffsetG()] = Image<T>::convertValue( m_data[pixelOffset], U() );
				data[i + out->getOffsetR()] = Image<T>::convertValue( m_data[pixelOffset], U() );
				// if output image has alpha channel
				if ( outNumChan == 4 )
				{
					data[i + out->getOffsetA()] = Image<T>::ValueMax;
				}

				++pixelOffset;
			}
		}
		else if ( m_numChan == 3 && outNumChan == 4 )
		{
			int pixelOffset = 0;
			for ( int i = 0; i < outMemAllocated; i += outNumChan )
			{
				data[i + out->getOffsetB()] = Image<T>::convertValue( m_data[pixelOffset + m_offsetB], U() );
				data[i + out->getOffsetG()] = Image<T>::convertValue( m_data[pixelOffset + m_offsetG], U() );
				data[i + out->getOffsetR()] = Image<T>::convertValue( m_data[pixelOffset + m_offsetR], U() );
				data[i + out->getOffsetA()] = Image<T>::ValueMax;;

				pixelOffset += m_numChan;
			}
		}
	}
}

template <typename T, ColorSpace C>
int ColorImage<T, C>::getNumberOfChannels() const
{ 
	return m_numChan;
}

template <typename T, ColorSpace C>
unsigned int ColorImage<T, C>::getOffsetR() const { 
	return m_offsetR; 
}

template <typename T, ColorSpace C>
unsigned int ColorImage<T, C>::getOffsetG() const {
	return m_offsetG; 
}

template <typename T, ColorSpace C>
unsigned int ColorImage<T, C>::getOffsetB() const {
	return m_offsetB; 
}

template <typename T, ColorSpace C>
unsigned int ColorImage<T, C>::getOffsetA() const {
	return m_offsetA; 
}

/** ------------------------------------
*					private
*	------------------------------------
**/

template <typename T, ColorSpace C>
bool ColorImage<T, C>::readCV( const std::string &fileName )
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
	reallocateMemory( cvImage.size().width, cvImage.size().height, cvImage.channels() );
	
	// Copy data over from cvImage
	int numColumns = cvImage.size().width * cvImage.channels();
	int step = (int)cvImage.step;

	unsigned char* data = cvImage.data;
	int pixelCount = 0;

	if ( cvImage.channels() == 1 )
	{
		for ( int i = 0; i < m_height; ++i )
		{
			for ( int j = 0; j < numColumns; ++j )
			{
				m_data[pixelCount] = Image<unsigned char>::convertValue( data[j], T() );
				pixelCount++;
			}
			data += step; // next line
		}
	}
	else if ( cvImage.channels() == 3 || cvImage.channels() == 4 )
	{
		for ( int i = 0; i < m_height; ++i )
		{
			for ( int j = 0; j < numColumns; j += cvImage.channels() )
			{
				m_data[B( pixelCount )] = Image<unsigned char>::convertValue( data[j], T() );
				m_data[G( pixelCount )] = Image<unsigned char>::convertValue( data[j + 1], T() );
				m_data[R( pixelCount )] = Image<unsigned char>::convertValue( data[j + 2], T() );
				if ( cvImage.channels() == 4 )
				{
					m_data[A( pixelCount )] = Image<unsigned char>::convertValue( data[j + 3], T() );
				}
				pixelCount++;
			}
			data += step; // next line
		}
	}

	// openCV cv::Mat handles memory autamtically -> no destructor call needed.
	return true;
}

template< typename T, ColorSpace C>
void ColorImage<T, C>::writeCV( const std::string& fileName ) const
{
	T* trgData = new T[m_memAllocated];

	if ( m_numChan == 1 )
	{
		for ( int i = 0; i < m_memAllocated; ++i )
		{
			trgData[i] = Image<T>::convertValue( m_data[i], unsigned char() );
		}
	}
	else if ( m_numChan == 3 || m_numChan == 4 )
	{
		for ( int i = 0; i < m_memAllocated; i += m_numChan )
		{
			trgData[i] = Image<T>::convertValue( m_data[i + m_offsetB], unsigned char() );
			trgData[i + 1] = Image<T>::convertValue( m_data[i + m_offsetG], unsigned char() );
			trgData[i + 2] = Image<T>::convertValue( m_data[i + m_offsetR], unsigned char() );
			if ( m_numChan == 4 )
			{
				trgData[i + 3] = Image<T>::convertValue( m_data[i + m_offsetA], unsigned char() );
			}
		}
	}

	writeCVHelper( fileName, trgData );

	delete[] trgData;
}

template <>
void ColorImage<float, ColorSpace::CS_GRAY>::writeCVHelper( const std::string& fileName, float tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC1, tmp ) );
}

template <>
void ColorImage<float, ColorSpace::CS_RGB>::writeCVHelper( const std::string& fileName, float tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC3, tmp ) );
}

template <>
void ColorImage<float, ColorSpace::CS_HSV>::writeCVHelper( const std::string& fileName, float tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC3, tmp ) );
}

template <>
void ColorImage<float, ColorSpace::CS_LAB>::writeCVHelper( const std::string& fileName, float tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC3, tmp ) );
}

template <>
void ColorImage<float, ColorSpace::CS_BGR>::writeCVHelper( const std::string& fileName, float tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC3, tmp ) );
}

template <>
void ColorImage<float, ColorSpace::CS_RGBA>::writeCVHelper( const std::string& fileName, float tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC4, tmp ) );
}

template <>
void ColorImage<float, ColorSpace::CS_BGRA>::writeCVHelper( const std::string& fileName, float tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC4, tmp ) );
}

template <>
void ColorImage<float, ColorSpace::CS_ARGB>::writeCVHelper( const std::string& fileName, float tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_32FC4, tmp ) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_GRAY>::writeCVHelper( const std::string& fileName, unsigned char tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC1, tmp ) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_RGB>::writeCVHelper( const std::string& fileName, unsigned char tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC3, tmp ) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_HSV>::writeCVHelper( const std::string& fileName, unsigned char tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC3, tmp ) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_LAB>::writeCVHelper( const std::string& fileName, unsigned char tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC3, tmp ) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_BGR>::writeCVHelper( const std::string& fileName, unsigned char tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC3, tmp ) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_RGBA>::writeCVHelper( const std::string& fileName, unsigned char tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC4, tmp ) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_BGRA>::writeCVHelper( const std::string& fileName, unsigned char tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC4, tmp ) );
}

template <>
void ColorImage<unsigned char, ColorSpace::CS_ARGB>::writeCVHelper( const std::string& fileName, unsigned char tmp[] ) const
{
	cv::imwrite( fileName, cv::Mat( m_height, m_width, CV_8UC4, tmp ) );
}

template <typename T, ColorSpace C>
void ColorImage<T, C>::reallocateMemory( int width, int height, int numChan )
{
	int memNeeded = width * height * numChan;

	if ( m_memAllocated < memNeeded )
	{
		delete[] m_data;
		m_data = nullptr;

		setRGBAOffsets();

		m_memAllocated = m_numChan * m_width * m_height;
		std::cout << "Reallocated " << m_memAllocated << std::endl;
		m_data = new T[m_memAllocated];
	}
	else
	{
		std::cout << "No need to reallocate." << std::endl;
	}
}

template <typename T, ColorSpace C>
void ColorImage<T, C>::setRGBAOffsets()
{
	switch ( C )
	{
		case ColorSpace::CS_RGB :
		case ColorSpace::CS_HSV :
		case ColorSpace::CS_LAB :
			m_offsetR = 0;
			m_offsetG = 1;
			m_offsetB = 2;
			break;
		case ColorSpace::CS_GRAY :
			m_offsetR = m_offsetG = m_offsetB = m_offsetA = 0;
			break;
		case ColorSpace::CS_BGR :
			m_offsetR = 2;
			m_offsetG = 1;
			m_offsetB = 0;
			break;
		case ColorSpace::CS_RGBA :
			m_offsetR = 0;
			m_offsetG = 1;
			m_offsetB = 2;
			m_offsetA = 3;
			break;
		case ColorSpace::CS_BGRA :
			m_offsetR = 2;
			m_offsetG = 1;
			m_offsetB = 0;
			m_offsetA = 3;
			break;
		case ColorSpace::CS_ARGB :
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
