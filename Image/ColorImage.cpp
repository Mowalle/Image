/*
 *	This file must be excluded from build!
 */

template<ColorSpace C>
int ColorImage<unsigned char, C>::getNumberOfChannels(
    const ColorSpace colorSpace)
{
    switch (colorSpace)
    {
        case ColorSpace::CS_RGB:
        case ColorSpace::CS_BGR:
        case ColorSpace::CS_HSV:
        case ColorSpace::CS_LAB:
            return 3;
            break;
        case ColorSpace::CS_RGBA:
        case ColorSpace::CS_BGRA:
        case ColorSpace::CS_ARGB:
            return 4;
            break;
        case ColorSpace::CS_GRAY:
            return 1;
            break;
        default:
            return 0;
            break;
    }
}


template<ColorSpace C>
int ColorImage<float, C>::getNumberOfChannels(
    const ColorSpace colorSpace)
{
    switch (colorSpace)
    {
        case ColorSpace::CS_RGB:
        case ColorSpace::CS_BGR:
        case ColorSpace::CS_HSV:
        case ColorSpace::CS_LAB:
            return 3;
            break;
        case ColorSpace::CS_RGBA:
        case ColorSpace::CS_BGRA:
        case ColorSpace::CS_ARGB:
            return 4;
            break;
        case ColorSpace::CS_GRAY:
            return 1;
            break;
        default:
            return 0;
            break;
    }
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::getCvType(const ColorSpace colorSpace)
{
    switch (colorSpace)
    {
        case ColorSpace::CS_RGB:
        case ColorSpace::CS_BGR:
        case ColorSpace::CS_HSV:
        case ColorSpace::CS_LAB:
            return CV_8UC3;
            break;
        case ColorSpace::CS_RGBA:
        case ColorSpace::CS_BGRA:
        case ColorSpace::CS_ARGB:
            return CV_8UC4;
            break;
        case ColorSpace::CS_GRAY:
            return CV_8UC1;
            break;
        default:
            return 0;
            break;
    }
}


template<ColorSpace C>
int ColorImage<float, C>::getCvType(const ColorSpace colorSpace)
{
    switch (colorSpace)
    {
        case ColorSpace::CS_RGB:
        case ColorSpace::CS_BGR:
        case ColorSpace::CS_HSV:
        case ColorSpace::CS_LAB:
            return CV_32FC3;
            break;
        case ColorSpace::CS_RGBA:
        case ColorSpace::CS_BGRA:
        case ColorSpace::CS_ARGB:
            return CV_32FC4;
            break;
        case ColorSpace::CS_GRAY:
            return CV_32FC1;
            break;
        default:
            return 0;
            break;
    }
}


// =======================================================================================
// Public Member Functions
// ======================================================================================= 


// =================
// Con-/Desctructors
// =================

template<ColorSpace C>
ColorImage<unsigned char, C>::ColorImage() : m_channels(getNumberOfChannels(C))
{
    setRGBAOffsets();
    std::clog << "ColorImage default constructor" << std::endl;
}

template<ColorSpace C>
ColorImage<float, C>::ColorImage() : m_channels(getNumberOfChannels(C))
{
    setRGBAOffsets();
    std::clog << "ColorImage default constructor" << std::endl;
}


template<ColorSpace C>
ColorImage<unsigned char, C>::ColorImage(int width, int height)
    : Image(width, height),
	  m_channels(getNumberOfChannels(C))
{
    std::clog << "ColorImage width/height constructor" << std::endl;
    setRGBAOffsets();
    m_data.resize(size() * m_channels);
}

template<ColorSpace C>
ColorImage<float, C>::ColorImage(int width, int height) 
    : Image(width, height),
	  m_channels(getNumberOfChannels(C))
{
    std::clog << "ColorImage width/height constructor" << std::endl;
    setRGBAOffsets();
    m_data.resize(size() * m_channels);
}


template<ColorSpace C>
ColorImage<unsigned char, C>::ColorImage(const std::string& fileName) 
    : m_channels(getNumberOfChannels(C))
{
    std::clog << "ColorImage filename constructor" << std::endl;
    setRGBAOffsets();
    read(fileName);
}

template<ColorSpace C>
ColorImage<float, C>::ColorImage(const std::string& fileName) 
    : m_channels(getNumberOfChannels(C))
{
    std::clog << "ColorImage filename constructor" << std::endl;
    setRGBAOffsets();
    read(fileName);
}


template<ColorSpace C>
ColorImage<unsigned char, C>::~ColorImage()
{
    std::clog << "ColorImage destructor" << std::endl;
}


template<ColorSpace C>
ColorImage<float, C>::~ColorImage()
{
    std::clog << "ColorImage destructor" << std::endl;
}


template<ColorSpace C>
bool ColorImage<unsigned char, C>::read(const std::string& fileName)
{
    // Create dummy file and check if it was a success before trying to 
    // read image.
    std::ifstream dummy;
    // Opening dummy file to check if it exists at all.
    // Replaced the old C-directives (may be slower).
    dummy.open(fileName);

    bool throwsError = false;
    if (!dummy.is_open()) 
    {
        std::cerr << "ERROR: Could not open or find file " << fileName << "." << std::endl;
    	throwsError = true;
    } 
    else 
    {
        std::clog << "Trying to open file with OpenCV..." << std::endl;
        // read image using openCV
        throwsError = !readCv(fileName);
		
		// special conversion if image is in HSV-Space
		if (!throwsError && C == ColorSpace::CS_HSV)
			ColorImage::convertToHsv();
    }
    return throwsError;
}

template<ColorSpace C>
bool ColorImage<float, C>::read(const std::string& fileName)
{
    // Create dummy file and check for success before trying to read image.
    std::ifstream dummy;
    // Opening dummy file to check if it exists at all.
    // Replaced the old C-directives (may be slower).
    dummy.open(fileName);

    bool throwsError = false;
    if (!dummy.is_open())
    {
        std::cerr << "ERROR: Could not open or find file " << fileName << "." << std::endl;
        throwsError = true;
    }
    else
    {
        std::cout << "Trying to open file with OpenCV..." << std::endl;
        // read image using openCV
        throwsError = !readCv(fileName);

	// special conversion if image is in HSV- or Lab-Space
	if (!throwsError && C == ColorSpace::CS_HSV)
	    ColorImage::convertToHsv();
	else if (!throwsError && C == ColorSpace::CS_LAB)
	    ColorImage::convertToLab();
    }
    return throwsError;
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::write(const std::string& fileName) const
{
	if (m_width == 0 && m_height == 0)
	{
		std::cerr << "Image cannot be written to " << fileName << ", because it is empty!" << std::endl;
		return;
	}
	
    std::vector<unsigned char> data(size() * m_channels);

    // Convert to BGRA-space for OpenCV.
    for (unsigned i = 0; i < size(); ++i)
    {
        // For 1 channel outside if, because grey-channel values
        // are the same in each channel; does not matter which to copy.
        data[(i * m_channels)] = m_data[B(i)];
        if (m_channels >= 3)
        {
            data[(i * m_channels) + 1] = m_data[G(i)];
            data[(i * m_channels) + 2] = m_data[R(i)];
        }
        if (m_channels == 4)
            data[(i * m_channels) + 3] = m_data[A(i)];
    }

    cv::Mat output(m_height, m_width, getCvType(C), data.data());
    cv::imwrite(fileName, output);
}

template<ColorSpace C>
void ColorImage<float, C>::write(const std::string& fileName) const
{
	if (m_width == 0 && m_height == 0)
	{
		std::cerr << "Image cannot be written to " << fileName << ", because it is empty!" << std::endl;
		return;
	}

    std::vector<float> data(size() * m_channels);

    // Convert to BGRA-space for OpenCV.
    for (unsigned i = 0; i < size(); ++i)
    {
        // For 1 channel outside if, because grey-channel values
        // are the same in each channel; does not matter which to copy.
        data[(i * m_channels)] = m_data[B(i)] * 255.0f;
        if (m_channels >= 3)
        {
            data[(i * m_channels) + 1] = m_data[G(i)] * 255.0f;
            data[(i * m_channels) + 2] = m_data[R(i)] * 255.0f;
        }
        if (m_channels == 4)
            data[(i * m_channels) + 3] = m_data[A(i)] * 255.0f;
    }

    cv::Mat output(m_height, m_width, getCvType(C), data.data());
    cv::imwrite(fileName, output);
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::copyTo(ColorImage* output) const
{
	output->m_width = m_width;
	output->m_height = m_height;
	output->m_data = m_data;
}

template<ColorSpace C>
void ColorImage<float, C>::copyTo(ColorImage* output) const
{
    output->m_width = m_width;
	output->m_height = m_height;
	output->m_data = m_data;
}


// TODO: Method is massive, cut size down somehow.
template<ColorSpace C> template<ColorSpace D>
void ColorImage<unsigned char, C>::convertColorSpace(ColorImage<unsigned char, D>* output) const
{
    output->m_width = m_width;
    output->m_height = m_height;

	output->m_data.resize(m_width * m_height * output->m_channels);

    if (C == D)
	{
        std::copy(m_data.begin(), m_data.end(), output->m_data.begin());
		return;
    }	
	
	for (unsigned i = 0; i < size(); ++i)
	{
		if (m_channels == output->m_channels)	// if both images have same number of channels
		{
			output->setPixelColor(i, m_data[R(i)], 
									 m_data[G(i)], 
									 m_data[B(i)], 
									 m_data[A(i)]);
		}
		else if (output->m_channels == 1)		// input.m_channels is implicitly greater than output's
		{
			float averageColor = 0;
			// Calculate average color value and assign it.
			// TODO: Other weights (luminosity etc.) instead of just average color.
			averageColor = (  m_data[B(i)] 
							+ m_data[G(i)] 
							+ m_data[R(i)]) / 3.0f;
			unsigned char avgColor = static_cast<unsigned char>(averageColor);
			output->setPixelColor(i, avgColor, avgColor, avgColor);
		}
		else									// in all other cases the alpha is either dismissed or set to default value
		{
			output->setPixelColor(i, m_data[R(i)], m_data[G(i)], m_data[B(i)]);
		}	
	}    

    // If needed, convert from/to HSV based system.
    if (C == ColorSpace::CS_HSV && D != ColorSpace::CS_HSV)
        output->convertFromHsv();
	else if (C != ColorSpace::CS_HSV && D == ColorSpace::CS_HSV)
        output->convertToHsv();
}


// TODO: Method is massive, cut size down somehow.
template<ColorSpace C> template<ColorSpace D>
void ColorImage<float, C>::convertColorSpace(
    ColorImage<float, D>* output) const
{
    output->m_width = m_width;
    output->m_height = m_height;

	output->m_data.resize(m_width * m_height * output->m_channels);

    if (C == D)
	{
        std::copy(m_data.begin(), m_data.end(), output->m_data.begin());
		return;
    }	
	
	for (unsigned i = 0; i < size(); ++i)
	{
		if (m_channels == output->m_channels)	// if both images have same number of channels
		{
			output->setPixelColor(i, m_data[R(i)], 
									 m_data[G(i)], 
									 m_data[B(i)], 
									 m_data[A(i)]);
		}
		else if (output->m_channels == 1)		// input.m_channels is implicitly greater than output's
		{
			float averageColor = 0;
			// Calculate average color value and assign it.
			// TODO: Other weights (luminosity etc.) instead of just average color.
			averageColor = (  m_data[B(i)] 
							+ m_data[G(i)] 
							+ m_data[R(i)]) / 3.0f;
			output->setPixelColor(i, averageColor, averageColor, averageColor);
		}
		else									// in all other cases the alpha is either dismissed or set to default value
		{
			output->setPixelColor(i, m_data[R(i)], m_data[G(i)], m_data[B(i)]);
		}	
	}  

    // If needed, convert from/to HSV based system or from/to Lab.
    if (C == ColorSpace::CS_HSV && D != ColorSpace::CS_HSV)
        output->convertFromHsv();
    else if (C != ColorSpace::CS_HSV && D == ColorSpace::CS_HSV)
        output->convertToHsv();
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::convertType(
    ColorImage<float, C>* output) const
{
    output->m_width = m_width;
    output->m_height = m_height;
	output->m_data.resize(m_width * m_height * m_channels);

    for (int i = 0; i < output->m_data.size(); ++i)
        output->m_data[i] = m_data[i] / 255.0f;
}


template<ColorSpace C>
void ColorImage<float, C>::convertType(
    ColorImage<unsigned char, C>* output) const
{
    output->m_width = m_width;
    output->m_height = m_height;
	output->m_data = m_data;

    for (int i = 0; i < output->m_data.size(); ++i)
        output->m_data[i] = static_cast<unsigned char>(m_data[i] * 255.0f);
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::resize(int width, int height)
{
    m_width = width;
    m_height = height;
	m_data.resize(m_width * m_height * m_channels);
}

template<ColorSpace C>
void ColorImage<float, C>::resize(int width, int height)
{
    m_width = width;
    m_height = height;
	m_data.resize(m_width * m_height * m_channels);
}


template<ColorSpace C> template<typename U, ColorSpace D>
bool ColorImage<unsigned char, C>::sameSize(
	const ColorImage<U, D>& other) const
{
    if (m_width == other.getWidth() && m_height == other.getHeight())
        return true;
	
	return false;
}

template<ColorSpace C> template<typename U, ColorSpace D>
bool ColorImage<float, C>::sameSize(
    const ColorImage<U, D>& other) const
{
	if (m_width == other.getWidth() && m_height == other.getHeight())
		return true;
	
	return false;
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::setPixelColor(int pixelIdx,
										 unsigned char r,
										 unsigned char g /* = r */,
										 unsigned char b /* = r */,
										 unsigned char a /* = 255 */)
{
	m_data[B(pixelIdx)] = b;
	if (m_channels >= 3) 
	{
		m_data[G(pixelIdx)] = g;
		m_data[R(pixelIdx)] = r;
	}
	if (m_channels == 4)
		m_data[A(pixelIdx)] = a;
}

template<ColorSpace C>
void ColorImage<float, C>::setPixelColor(int pixelIdx,
										 float r,
										 float g /* = r */,
										 float b /* = r */,
										 float a /* = 1.0f */)
{
	m_data[B(pixelIdx)] = b;
	if (m_channels >= 3) 
	{
		m_data[G(pixelIdx)] = g;
		m_data[R(pixelIdx)] = r;
	}
	if (m_channels == 4)
		m_data[A(pixelIdx)] = a;
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::setToBlack()
{
    setToValue(0, 0, 0, 255);
}


template<ColorSpace C>
void ColorImage<float, C>::setToBlack()
{
    setToValue(0, 0, 0, 1.0f);
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::setToValue(unsigned char value)
{
    setToValue(value, value, value);
}


template<ColorSpace C>
void ColorImage<float, C>::setToValue(float value)
{
    setToValue(value, value, value);
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::setToValue(unsigned char r,
                                              unsigned char g,
                                              unsigned char b,
                                              unsigned char a /* = 255 */)
{
    for (unsigned i = 0; i < size(); ++i) 
        setPixelColor(i, r, g, b, a);
}


template<ColorSpace C>
void ColorImage<float, C>::setToValue(float r,
                                      float g,
                                      float b,
                                      float a /* = 1.0f */) 
{
    for (unsigned i = 0; i < size(); i += m_channels) 
        setPixelColor(i, r, g, b, a);
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::R(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetR;
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::R(int x, int y) const
{
    return R((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<float, C>::R(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetR;
}


template<ColorSpace C>
int ColorImage<float, C>::R(int x, int y) const
{
    return R((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::G(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetG;
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::G(int x, int y) const
{
    return G((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<float, C>::G(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetG;
}


template<ColorSpace C>
int ColorImage<float, C>::G(int x, int y) const
{
    return G((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::B(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetB;
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::B(int x, int y) const
{
    return B((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<float, C>::B(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetB;
}


template<ColorSpace C>
int ColorImage<float, C>::B(int x, int y) const
{
    return B((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::A(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetA;
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::A(int x, int y) const
{
    return A((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<float, C>::A(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetA;
}


template<ColorSpace C>
int ColorImage<float, C>::A(int x, int y) const
{
    return A((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::H(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetR;
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::H(int x, int y) const
{
    return H((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<float, C>::H(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetR;
}


template<ColorSpace C>
int ColorImage<float, C>::H(int x, int y) const
{
    return H((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::S(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetG;
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::S(int x, int y) const
{
    return S((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<float, C>::S(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetG;
}


template<ColorSpace C>
int ColorImage<float, C>::S(int x, int y) const
{
    return S((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::V(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetB;
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::V(int x, int y) const
{
    return V((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<float, C>::V(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetB;
}


template<ColorSpace C>
int ColorImage<float, C>::V(int x, int y) const
{
    return V((y * m_width) + x);
}


template<ColorSpace C>
int ColorImage<unsigned char, C>::getChannels() const
{
    return m_channels;
}


template<ColorSpace C>
int ColorImage<float, C>::getChannels() const
{
    return m_channels;
}


template<ColorSpace C>
unsigned int ColorImage<unsigned char, C>::getOffsetR() const
{
    return m_offsetR;
}


template<ColorSpace C>
unsigned int ColorImage<float, C>::getOffsetR() const
{
    return m_offsetR;
}


template<ColorSpace C>
unsigned int ColorImage<unsigned char, C>::getOffsetG() const
{
    return m_offsetG;
}


template<ColorSpace C>
unsigned int ColorImage<float, C>::getOffsetG() const
{
    return m_offsetG;
}


template<ColorSpace C>
unsigned int ColorImage<unsigned char, C>::getOffsetB() const
{
    return m_offsetB;
}


template<ColorSpace C>
unsigned int ColorImage<float, C>::getOffsetB() const
{
    return m_offsetB;
}


template<ColorSpace C>
unsigned int ColorImage<unsigned char, C>::getOffsetA() const
{
    return m_offsetA;
}


template<ColorSpace C>
unsigned int ColorImage<float, C>::getOffsetA() const
{
    return m_offsetA;
}

// =======================================================================================
// Private Member Functions
// ======================================================================================= 

template<ColorSpace C>
void ColorImage<unsigned char, C>::convertToHsv()
{
    float h, s, v;
	unsigned size = this->size();
    for (unsigned i = 0; i < size; ++i)
    {
        convertColorToHsv(m_data[R(i)] / 255.0f, // Since conversion-function
                          m_data[G(i)] / 255.0f, // takes floats, we must di-
                          m_data[B(i)] / 255.0f, // vide first by 255.
                          &h, &s, &v);

        /* Since the data is in unsigned char, we must map the floats accor-
         * dingly. Saturation and value shall be in range [0,255], and hue in
         * range [0,180] (because unsigned char cannot go to 360) => this fol-
         * lows the OpenCV-convention for HSV. */
        m_data[R(i)] = static_cast<unsigned char> (h / 2);
        m_data[G(i)] = static_cast<unsigned char> (s * 255);
        m_data[B(i)] = static_cast<unsigned char> (v * 255);
    }
}


template<ColorSpace C>
void ColorImage<float, C>::convertToHsv()
{
    float h, s, v;
	unsigned size = this->size();
    for (unsigned i = 0; i < size; ++i)
    {
        convertColorToHsv(m_data[R(i)], 
						  m_data[G(i)],
						  m_data[B(i)],
						  &h, &s, &v);

        m_data[R(i)] = h / 360.0f;  // Normalizing the hue value.
        m_data[G(i)] = s;
        m_data[B(i)] = v;
    }
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::convertFromHsv()
{
    float r, g, b;
    for (unsigned i = 0; i < m_width * m_height; ++i)
    {
        convertColorFromHsv(m_data[H(i)] * 2.0f, // Conversion function ex-
                            m_data[S(i)] / 255.0f, // pects hue in range from
                            m_data[V(i)] / 255.0f, // [0,360] and s,v from 
                            &r, &g, &b); // [0,1].

        // Since the data is in unsigned char, we must map the floats
        // accordingly.
        m_data[R(i)] = static_cast<unsigned char> (r * 255);
        m_data[G(i)] = static_cast<unsigned char> (g * 255);
        m_data[B(i)] = static_cast<unsigned char> (b * 255);
    }
}


template<ColorSpace C>
void ColorImage<float, C>::convertFromHsv()
{
    float r, g, b;
    for (unsigned i = 0; i < size(); ++i)
    {
        convertColorFromHsv(m_data[H(i)] * 360.0f,
                            m_data[S(i)],
                            m_data[V(i)],
                            &r, &g, &b);

        // Since the data is in unsigned char, we must map the floats
        // accordingly.
        m_data[R(i)] = r;
        m_data[G(i)] = g;
        m_data[B(i)] = b;
    }
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::convertColorToHsv(float r, float g,
                                                     float b, float* h,
                                                     float* s, float* v)
{

    // This code is based on Daniel Mohr's version, which again is based on the
    // approach presented in Computer Graphics: Principles and Practive. Foley
    // et al.
    // Given: r, g, b, each in [0, 1].
    // Desired: h in [0, 360), s and v in [0, 1], except if s = 0,
    // then h = UNDEFINED. 
    float max = std::max(std::max(r, g), b);
    float min = std::min(std::min(r, g), b);

    *v = max;

    // Calculate saturation value
    if (max != 0.0f)
        *s = (max - min) / max;
    else
        *s = 0.0f;

    // Calculate hue value
    if (*s == 0.0f)	
	{
        // Hue is undefined here, so we just assign 0.
        *h = 0.0f;
	} 
	else 
	{
        float delta = max - min;
        if (r == max)
            *h = (g - b) / delta;
        else if (g == max)
            *h = 2.0f + (b - r) / delta;
        else if (b == max)
            *h = 4.0f + (r - g) / delta;

        *h *= 60.0f;
        if (*h < 0.0f)
            *h += 360.0f;
    }
}


template<ColorSpace C>
void ColorImage<float, C>::convertColorToHsv(float  r, float  g, float  b,
                                             float* h, float* s, float* v)
{
    // This code is based on Daniel Mohr's version, which again is based on the
    // approach presented in Computer Graphics: Principles and Practive. Foley
    // et al.
    // Given: r, g, b, each in [0, 1].
    // Desired: h in [0, 360), s and v in [0, 1], except if s = 0,
    // then h = UNDEFINED. 
    float max = std::max(std::max(r, g), b);
    float min = std::min(std::min(r, g), b);

    *v = max;

    // Calculate saturation value
    if (max != 0.0f)
        *s = (max - min) / max;
    else
        *s = 0.0f;

    // Calculate hue value
    if (*s == 0.0f)
    {
        // Hue is undefined here, so we just assign 0.
        *h = 0.0f;
    } 
	else
    {
        float delta = max - min;
        if (r == max)
            *h = (g - b) / delta;
        else if (g == max)
            *h = 2.0f + (b - r) / delta;
        else if (b == max)
            *h = 4.0f + (r - g) / delta;

        *h *= 60.0f;
        if (*h < 0.0f)
            *h += 360.0f;
    }
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::convertColorFromHsv(float h,
													   float s,
                                                       float v,
													   float* r,
                                                       float* g,
													   float* b)
{
    /* This code is based on Daniel Mohr's version, which again is based on
    the approach presented in Computer Graphics: Principles and
    Practive. Foley et al. */
    /* Given: h in [0, 360) or UNDEFINED, s and v in [0, 1] */
    /* Desired: r, g, b, each in [0, 1]. */
    if (s == 0)
    {
        if (h == -1.0)
        {
            *r = v;
            *g = v;
            *b = v;
        } 
		else
        {
            std::cerr << "HSV -> RGB conversion error (s == 0 and h > 0)\n" 
					  << std::endl;
            exit(1);
        }
    } 
	else
    {
        float f, p, q, t;
        int i;

        if (h == 360.0)
            h = 0.0;

        h /= 60;
        i = static_cast<int> (std::floor(h));
        f = h - i;
        p = v * (1.0f - s);
        q = v * (1.0f - (s * f));
        t = v * (1.0f - (s * (1.0f - f)));

        switch (i)
        {
            case 0:
                *r = v;
                *g = t;
                *b = p;
                break;
            case 1:
                *r = q;
                *g = v;
                *b = p;
                break;
            case 2:
                *r = p;
                *g = v;
                *b = t;
                break;
            case 3:
                *r = p;
                *g = q;
                *b = v;
                break;
            case 4:
                *r = t;
                *g = p;
                *b = v;
                break;
            case 5:
                *r = v;
                *g = p;
                *b = q;
                break;
        }
    }
}


template<ColorSpace C>
void ColorImage<float, C>::convertColorFromHsv(float  h, float  s, float  v,
                                               float* r, float* g, float* b)
{
    // This code is based on Daniel Mohr's version, which again is based on
    // the approach presented in Computer Graphics: Principles and
    // Practive. Foley et al.
    // Given: h in [0, 360) or UNDEFINED, s and v in [0, 1].
    // Desired: r, g, b, each in [0, 1].
    if (s == 0)
    {
        if (h == -1.0)
        {
            *r = v;
            *g = v;
            *b = v;
        } 
		else
        {
            std::cout << "HSV -> RGB conversion error (s == 0 and h> 0)\n" <<
                std::endl;
            exit(1);
        }
    } 
	else
    {
        float f, p, q, t;
        int i;

        if (h == 360.0)
        {
            h = 0.0;
        }

        h /= 60;
        i = static_cast<int> (std::floor(h));
        f = h - i;
        p = v * (1.0f - s);
        q = v * (1.0f - (s * f));
        t = v * (1.0f - (s * (1.0f - f)));

        switch (i)
        {
            case 0:
                *r = v;
                *g = t;
                *b = p;
                break;
            case 1:
                *r = q;
                *g = v;
                *b = p;
                break;
            case 2:
                *r = p;
                *g = v;
                *b = t;
                break;
            case 3:
                *r = p;
                *g = q;
                *b = v;
                break;
            case 4:
                *r = t;
                *g = p;
                *b = v;
                break;
            case 5:
                *r = v;
                *g = p;
                *b = q;
                break;
        }
    }
}


// Based on Daniel Mohr's implementation.
template<ColorSpace C>
void ColorImage<float, C>::convertToLab()
{
    float lab[3];
    float min[3] ={std::numeric_limits<float>::max()};
    float max[3] ={-std::numeric_limits<float>::max()};

    int numPixels = m_width * m_height;
    for (int i = 0; i < numPixels; ++i)
    {
        convertColorToLab(m_data[R(i)],
						  m_data[G(i)],
						  m_data[B(i)],
                          &lab[0],
						  &lab[1],
						  &lab[2]);

        m_data[R(i)] = lab[0];
        m_data[G(i)] = lab[1];
        m_data[B(i)] = lab[2];
    }
}


// Based on Daniel Mohr's implementation.
template<ColorSpace C>
void ColorImage<float, C>::convertFromLab()
{
    float r, g, b;
    for (int i = 0; i < m_width * m_height; ++i)
    {
        convertColorFromLab(m_data[R(i)],
							m_data[G(i)],
							m_data[B(i)],
                            &r, &g, &b);

        // Since the data is in unsigned char, we must map the floats
        // accordingly.
        m_data[R(i)] = r;
        m_data[G(i)] = g;
        m_data[B(i)] = b;
    }
}

// Based on Daniel Mohr's / Wikipedia's implementation.
template<ColorSpace C>
void ColorImage<float, C>::convertColorToLab(float r, float g, float b,
                                             float* lo, float* ao, float* bo)
{
    float x, y, z;
    // Converts r, g, b to x, y, z
    x = (0.412453f * r) + (0.357580f * g) + (0.180423f * b);
    y = (0.212671f * r) + (0.715160f * g) + (0.072169f * b);
    z = (0.019334f * r) + (0.119193f * g) + (0.950227f * b);

    // Reference white points.
    const float X_WHITE = 1.0f / 3.0f;
    const float Y_WHITE = 1.0f / 3.0f;
    const float Z_WHITE = 1.0f / 3.0f;

    x = x / X_WHITE;
    y = y / Y_WHITE;
    z = z / Z_WHITE;

    bool xr = x > 0.0088564517f;
    bool yr = y > 0.0088564517f;
    bool zr = z > 0.0088564517f;

    float fX, fY, fZ;
    fX = xr ? std::pow(x, (1.0f / 3.0f)) : (7.787f * x + (4.0f / 29.0f));
    fY = yr ? std::pow(y, (1.0f / 3.0f)) : (7.787f * y + (4.0f / 29.0f));
    fZ = zr ? std::pow(z, (1.0f / 3.0f)) : (7.787f * z + (4.0f / 29.0f));

    *lo = 116.0f * fY - 16.0f;
    *ao = 500.0f * (fX - fY);
    *bo = 200.0f * (fY - fZ);
}

// Based on Daniel Mohr's implementation.
template<ColorSpace C>
void ColorImage<float, C>::convertColorFromLab(float lIn, float aIn, float bIn,
                                               float* r, float* g, float* b)
{
    // Reference white points.
    const float X_WHITE = 1.0f / 3.0f;
    const float Y_WHITE = 1.0f / 3.0f;
    const float Z_WHITE = 1.0f / 3.0f;

    float x, y, z;
    y = (1.0f / 116.0f) * (lIn + 16.0f);
    x = (1.0f / 116.0f) * (lIn + 16.0f) + (1.0f / 500.0f) * aIn;
    z = (1.0f / 116.0f) * (lIn + 16.0f) - (1.0f / 200.0f) * bIn;

    bool xr = x > (6.0f / 29.0f);
    bool yr = y > (6.0f / 29.0f);
    bool zr = z > (6.0f / 29.0f);

    // Use inverse from function f from forward conversion.
    float fInverseX = xr ? 
		std::pow(x, 3.0f) : ((108.0f / 841.0f) * (x - (4.0f / 29.0f)));
    float fInverseY = yr ?
		std::pow(y, 3.0f) : ((108.0f / 841.0f) * (y - (4.0f / 29.0f)));
    float fInverseZ = zr ? 
		std::pow(z, 3.0f) : ((108.0f / 841.0f) * (y - (4.0f / 29.0f)));

    x = X_WHITE * fInverseX;
    y = Y_WHITE * fInverseY;
    z = Z_WHITE * fInverseZ;

    // Converts x, y, z into r, g, b.
    *r = (3.240479f * x) + (-1.53715f  * y) + (-0.498535f * z);
    *g = (-0.969256f * x) + (1.875991f * y) + (0.041556f * z);
    *b = (0.055648f * x) + (-0.204043f * y) + (1.057311f * z);
}


template<ColorSpace C>
bool ColorImage<unsigned char, C>::readCv(const std::string& fileName)
{
    cv::Mat cvImage = cv::imread(fileName, cv::IMREAD_UNCHANGED);
    if (cvImage.empty())
    {
        std::cerr << "ERROR: OpenCV failed to load file " << fileName << ".\n";
        return false;
    }

    if (cvImage.channels() != m_channels)
    {
        std::cerr << "ERROR: Image does not have compatible number of channels!\n"
		  << "(Expected " << m_channels << " channels, but file has "
		  << cvImage.channels() << " channels.)" 
                  << std::endl;
        return false;
    }

    m_width  = cvImage.size().width;
    m_height = cvImage.size().height;
    m_data.resize(m_width * m_height * m_channels);

    unsigned char* data = cvImage.data;

    for (unsigned i = 0; i < size(); ++i) 
	{
        // BUG: Not actually a bug, but unnecessary: we pass some values
		// as parameters, even though they might not be used (e.g. alpha value
		// in 3-channel-image.
		setPixelColor(i, data[2], data[1], data[0], data[3]);	// in this order because cvIMage is BGR(A) space
		data += m_channels;
	}

    return true;
}


template<ColorSpace C>
bool ColorImage<float, C>::readCv(const std::string& fileName)
{
    cv::Mat cvImage = cv::imread(fileName, cv::IMREAD_UNCHANGED);
    if (cvImage.empty())
    {
        std::cerr << "ERROR: OpenCV failed to load file " << fileName << ".\n";
        return false;
    }

    if (cvImage.channels() != m_channels)
    {
        std::cerr << "ERROR: Image does not have compatible number of channels!\n"
		  << "(Expected " << m_channels << " channels, but file has "
		  << cvImage.channels() << " channels.)" 
                  << std::endl;
        return false;
    }

    m_width  = cvImage.size().width;
    m_height = cvImage.size().height;
    m_data.resize(m_width * m_height * m_channels);

    unsigned char* data = cvImage.data;
    // TODO(Ralf): Was needed before, now it reads them incorrectly if this is enabled?
    //// step alignment into next scan line
    // int step = static_cast<int>(cvImage.step);

    for (unsigned i = 0; i < size(); ++i) 
	{
        // BUG: Not actually a bug, but unnecessary: we pass some values
		// as parameters, even though they might not be used (e.g. alpha value
		// in 3-channel-image.
		setPixelColor(i, data[2] / 255.0f, 
			             data[1] / 255.0f, 
						 data[0] / 255.0f, 
						 data[3] / 255.0f);	// in this order because cvIMage is BGR(A) space
		data += m_channels;
	}

    return true;
}


template<ColorSpace C>
void ColorImage<unsigned char, C>::setRGBAOffsets()
{
    switch (C)
    {
        case ColorSpace::CS_RGB:
        case ColorSpace::CS_HSV:
        case ColorSpace::CS_LAB:
            m_offsetR = 0;
            m_offsetG = 1;
            m_offsetB = 2;
            break;
        case ColorSpace::CS_GRAY:
            m_offsetR = m_offsetG = m_offsetB = m_offsetA = 0;
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


template<ColorSpace C>
void ColorImage<float, C>::setRGBAOffsets()
{
    switch (C)
    {
        case ColorSpace::CS_RGB:
        case ColorSpace::CS_HSV:
        case ColorSpace::CS_LAB:
            m_offsetR = 0;
            m_offsetG = 1;
            m_offsetB = 2;
            break;
        case ColorSpace::CS_GRAY:
            m_offsetR = m_offsetG = m_offsetB = m_offsetA = 0;
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

/**
 *	FLOAT
 **/
















































// -------
// PRIVATE
// -------














