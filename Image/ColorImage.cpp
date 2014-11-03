/**
*	This file must be excluded from build!
**/

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::read( const std::string &fileName )
{
	// Create dummy file and check if it was a success before trying to 
	// read image.
	FILE *dummy = fopen( fileName.c_str(), "r" );

	if ( dummy == nullptr )
	{
		fprintf( stderr, "ERROR: Couldn't open or find file %s.\n", fileName.c_str() ); // using fileName.c_str(), else the file name would be displayed incorretly.
		return false;
	}

	fclose(dummy);

	bool success;
	// Look at file name. Differentiate between formats.
	if ( isFileType( fileName, ".raw" ) )
	{
		std::cout << "Trying to open .raw-file..." << std::endl;
		success = readRaw( fileName );
	}
	else if ( isFileType( fileName, ".bin" ) )
	{
		std::cout << "Trying to open .bin-file..." << std::endl;
		success = readBinary( fileName );
	}
	else
	{
		std::cout << "Trying to open OpenCV-file..." << std::endl;
		// Use OpenCV to read image, replacing readCV(fileName, cf)
		success = readCV( fileName );
	}

	return success;
}

/** ------------------------------------
*					private
*	------------------------------------
**/

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::isFileType( const std::string &fileName , const std::string &fileType) const
{
	bool isEqual = false;

	// Get position of extension-point and compare file extensions.
	std::size_t pos = fileName.find( '.' );
	
	// if point was found
	if ( pos != std::string::npos )
	{
		// string::npos indicates all string until end of string filename.
		if ( fileName.compare ( pos, std::string::npos, fileType ) == 0 )
		{
			isEqual = true;
		}
	}

	return isEqual;
}

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::readBinary( const std::string &fileName )
{
	return false;
}

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::readCV( const std::string &fileName )
{
	// Load image
	cv::Mat cvImage = cv::imread( fileName, cv::IMREAD_UNCHANGED);

	// If image wasn't loaded (wrong format, etc.), exit
	if ( !cvImage.data )
	{
		fprintf( stderr, "ERROR: OpenCV failed to load file %s.\n", fileName.c_str() );
		return false;
	}

	// TODO: Option "Disregard alpha"
	if ( cvImage.channels() != m_numChan )
	{
		fprintf( stderr, "ERROR: Image doesn't have compatible number of channels!\n", fileName.c_str() );
		printf( "Set channels to %i, but loaded image has %i.\n", m_numChan, cvImage.channels() );
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
bool ColorImage<Type, cs>::readRaw( const std::string &fileName )
{
	return false;
}

template <typename Type, ColorSpace cs>
void ColorImage<Type, cs>::copyDataFromCV( const cv::Mat &cvImage )
{
	int numColumns = cvImage.size().width * cvImage.channels();
	int step = cvImage.step;

	uchar* data = cvImage.data;
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
			for ( int j = 0; j < numColumns; ++j )
			{
				m_data[b( pixelCount )] = data[j];
				m_data[g( pixelCount )] = data[j + 1];
				m_data[r( pixelCount )] = data[j + 2];
				pixelCount++;
			}
			data += step; // next line
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
	printf("Allocated %i\n", m_memAllocated);
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