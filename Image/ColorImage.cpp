/**
 *	This file must be excluded from build!
 **/

template<ColorSpace C>
int ColorImage< unsigned char, C >::getNumberOfChannels(
        const ColorSpace colorSpace)
{
    switch (colorSpace) {
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



// ------
// PUBLIC
// ------



// Constructors

template<ColorSpace C >
ColorImage< unsigned char, C >::ColorImage() :
Image(),
m_channels(getNumberOfChannels(C))
{
    setRGBAOffsets();
    std::cout << "ColorImage default constructor" << std::endl;
}

template<ColorSpace C>
ColorImage< unsigned char, C >::ColorImage(int width, int height) :
Image(width, height),
m_channels(getNumberOfChannels(C))
{
    setRGBAOffsets();
    m_allocatedMemory = m_width * m_height * m_channels;
    m_data = m_allocatedMemory ? new unsigned char[m_allocatedMemory] : nullptr;
    std::cout << "ColorImage width/height constructor" << std::endl;
}

template<ColorSpace C >
ColorImage< unsigned char, C >::ColorImage(const std::string& fileName) :
Image(),
m_channels(getNumberOfChannels(C))
{
    setRGBAOffsets();
    // allocateMemory() call later in readCv, since width and height have to be
    // read from file first.
    read(fileName);
    std::cout << "ColorImage filename constructor" << std::endl;
}

template<ColorSpace C >
ColorImage< unsigned char, C >::ColorImage(const ColorImage& other) :
Image(other),
m_channels(other.m_channels),
m_offsetR(other.m_offsetR), m_offsetG(other.m_offsetG),
m_offsetB(other.m_offsetB), m_offsetA(other.m_offsetA)
{
}


// UNDONE: Move Constructor
//template<ColorSpace C>
//ColorImage<unsigned char, C>::ColorImage(ColorImage&& other) :
//    Image(other),
//    m_channels(other.m_channels),
//    m_offsetR(other.m_offsetR), m_offsetG(other.m_offsetG), 
//    m_offsetB(other.m_offsetB), m_offsetA(other.m_offsetA)
//{
//    // Do not reset other here, values like offset and channels are tied to template.
//}

// Destructor

template<ColorSpace C >
ColorImage< unsigned char, C >::~ColorImage()
{
    std::cout << "ColorImage destructor" << std::endl;
}

template<ColorSpace C >
ColorImage<unsigned char, C>& ColorImage<unsigned char, C>::operator=(
        const ColorImage& rhs)
{
    if (this != &rhs) {
        // No need for deleting current offsets, will stay the same anyway, tied to template.
        Image::operator=(rhs);
    }
    return *this;
}


// UNDONE: Move Assignment Operator
//template<ColorSpace C>
//ColorImage<unsigned char, C>& ColorImage<unsigned char, C>::operator =(
//    ColorImage&& rhs)
//{
//    if (this != &rhs)
//    {
//        // No need for deleting current offsets, will stay the same anyway, tied to template.
//        Image::operator =(rhs);
//    }
//    return *this;
//}

template<ColorSpace C >
bool ColorImage< unsigned char, C >::read(const std::string& fileName)
{
    // Create dummy file and check if it was a success before trying to 
    // read image.
    std::ifstream dummy;
    // Opening dummy file to check if it exists at all.
    // Replaced the old C-directives (may be slower).
    dummy.open(fileName);

    bool throwsError = false;
    if (!dummy.is_open()) {
        std::cout << "ERROR: Could not open or find file " << fileName << ".\n";
        throwsError = true;
    }

    if (!throwsError) {
        std::cout << "Trying to open file with OpenCV..." << std::endl;
        // Actual Reading via OpenCV
        throwsError = readCv(fileName);
    }

    // UNDONE: What happens if C == CS_LAB?
    if (C == ColorSpace::CS_HSV) {
        ColorImage::convertToHsv();
    }

    return throwsError;
}

template<ColorSpace C >
void ColorImage< unsigned char, C >::write(const std::string& fileName) const
{
    int numValues = m_width * m_height * m_channels;
    unsigned char* data = new unsigned char[numValues];

    for (int i = 0; i < numValues; i += m_channels) {
        data[i] = m_data[i + m_offsetB]; // Works for gray (since offset = 0)
        if (m_channels >= 3) {
            data[i + 1] = m_data[i + m_offsetG];
            data[i + 2] = m_data[i + m_offsetR];
        }
        if (m_channels == 4) {
            data[i + 3] = m_data[i + m_offsetA];
        }
    }

    switch (C) {
        case ColorSpace::CS_RGB:
        case ColorSpace::CS_BGR:
        case ColorSpace::CS_HSV:
            cv::imwrite(fileName, cv::Mat(m_height, m_width, CV_8UC3, data));
            break;
        case ColorSpace::CS_RGBA:
        case ColorSpace::CS_BGRA:
        case ColorSpace::CS_ARGB:
            cv::imwrite(fileName, cv::Mat(m_height, m_width, CV_8UC4, data));
            break;
        case ColorSpace::CS_GRAY:
            cv::imwrite(fileName, cv::Mat(m_height, m_width, CV_8UC1, data));
            break;
    }

    delete[] data;
}

template<ColorSpace C >
void ColorImage< unsigned char, C >::copy(ColorImage* output) const
{
    // First compare width and height, than copy if the same.
    if (compareDimensions(*output)) {
        memcpy(output->m_data, m_data,
                m_allocatedMemory * sizeof (unsigned char));
    }
}

// TODO: Method is massive, cut size down somehow.

template<ColorSpace C > template<ColorSpace D>
void ColorImage< unsigned char, C >::convertColorSpace(
        ColorImage< unsigned char, D >* output) const
{
    output->m_width = m_width;
    output->m_height = m_height;

    // Allocate new memory if old data array is too small
    int neededMemory = m_width * m_height * output->getChannels();
    if (neededMemory > output->m_allocatedMemory) {
        delete[] output->m_data;
        output->m_data = new unsigned char[neededMemory];
        output->m_allocatedMemory = neededMemory;
    }

    if (C == D) {
        memcpy(output->m_data, m_data, neededMemory * sizeof (unsigned char));
    }
    else {
        // Case 1: Both images have same number of channels
        if (m_channels == output->m_channels) { 
            for (int i = 0; i < neededMemory; ++i) {
                // Simple memcpy() does not work here since channel order can be
                // different.
                output->m_data[i + output->m_offsetB] = m_data[i + m_offsetB];
                if (output->m_channels >= 3) {
                    output->m_data[i + output->m_offsetG] = m_data[i + m_offsetG];
                    output->m_data[i + output->m_offsetR] = m_data[i + m_offsetR];
                }
                if (output->m_channels == 4) {
                    output->m_data[i + output->m_offsetA] = m_data[i + m_offsetA];
                }
            }
        }
        // Case 2: source image has more channels
        else if (m_channels > output->m_channels) {
            // if output is grey image, convert.
            if (m_channels >= 3 && output->m_channels == 1) {
                int pixelOffset = 0;
                float averageColor = 0;

                for (int i = 0; i < neededMemory; ++i) {
                    // Calculate average color value and assign it.
                    // TODO: Other weights (luminosity etc.) instead of just average color.
                    averageColor = (m_data[pixelOffset + m_offsetB] +
                            m_data[pixelOffset + m_offsetG] +
                            m_data[pixelOffset + m_offsetR]) / 3.0f;
                    output->m_data[i] =
                            static_cast<unsigned char> (averageColor);
                    pixelOffset += m_channels;
                }
            } // If source has 4 channels and output only 3, strip off alpha.
            else if (m_channels == 4 && output->m_channels == 3) {
                int pixelOffset = 0;
                for (int i = 0; i < neededMemory; i += output->m_channels) {
                    output->m_data[i + output->m_offsetB] =
                            m_data[pixelOffset + m_offsetB];
                    output->m_data[i + output->m_offsetG] =
                            m_data[pixelOffset + m_offsetG];
                    output->m_data[i + output->m_offsetR] =
                            m_data[pixelOffset + m_offsetR];

                    pixelOffset += m_channels;
                }
            }
        }
        // Case 3: source image has less channels than output image
        else if (m_channels < output->m_channels) { 
            int pixelOffset = 0;
            for (int i = 0; i < neededMemory; i += output->m_channels) {
                output->m_data[i + output->m_offsetB] = m_data[pixelOffset + m_offsetB];
                output->m_data[i + output->m_offsetG] = m_data[pixelOffset + m_offsetG];
                output->m_data[i + output->m_offsetR] = m_data[pixelOffset + m_offsetR];
                
                // if output image has alpha channel
                if (output->m_channels == 4) {
                    output->m_data[i + output->m_offsetA] = 255;
                }

                pixelOffset += m_channels;
            }
        }
    }

    // If needed, convert from/to HSV based system.
    if (C == ColorSpace::CS_HSV && D != ColorSpace::CS_HSV) {
        output->convertFromHsv();
    }
    else if (C != ColorSpace::CS_HSV && D == ColorSpace::CS_HSV) {
        output->convertToHsv();
    }
}

template<ColorSpace C>
void ColorImage<unsigned char, C>::convertType(
        ColorImage<float, C>* output) const
{
    output->m_width = m_width;
    output->m_height = m_height;
    output->m_allocatedMemory = m_allocatedMemory;

    delete[] output->m_data;
    output->m_data = new float[m_allocatedMemory];

    for (int i = 0; i < m_allocatedMemory; ++i) {
        output->m_data[i] = m_data[i] / 255.0f;
    }
}

template<ColorSpace C > template<typename U, ColorSpace D >
bool ColorImage< unsigned char, C >::compareDimensions(
        const ColorImage< U, D >& other) const
{
    bool sameDimensions = false;
    if (m_width == other.getWidth()
            && m_height == other.getHeight()) {
        sameDimensions = true;
    }
    return sameDimensions;
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::R(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetR;
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::R(int x, int y) const
{
    return R((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::G(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetG;
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::G(int x, int y) const
{
    return G((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::B(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetB;
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::B(int x, int y) const
{
    return B((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::A(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetA;
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::A(int x, int y) const
{
    return A((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::H(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetR;
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::H(int x, int y) const
{
    return H((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::S(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetG;
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::S(int x, int y) const
{
    return S((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::V(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetB;
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::V(int x, int y) const
{
    return V((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage< unsigned char, C >::getChannels() const
{
    return m_channels;
}

template<ColorSpace C >
unsigned int ColorImage< unsigned char, C >::getOffsetR() const
{
    return m_offsetR;
}

template<ColorSpace C >
unsigned int ColorImage< unsigned char, C >::getOffsetG() const
{
    return m_offsetG;
}

template<ColorSpace C >
unsigned int ColorImage< unsigned char, C >::getOffsetB() const
{
    return m_offsetB;
}

template<ColorSpace C >
unsigned int ColorImage< unsigned char, C >::getOffsetA() const
{
    return m_offsetA;
}



// -------
// PRIVATE
// -------

template<ColorSpace C>
void ColorImage<unsigned char, C>::convertToHsv()
{
    float h, s, v;
    for (int i = 0; i < m_width * m_height; ++i) {
        convertColorToHsv(m_data[R(i)] / 255.0f, // Since conversion-function
                m_data[G(i)] / 255.0f, // takes floats, we must di-
                m_data[B(i)] / 255.0f, // vide first by 255.
                &h, &s, &v);

        /* Since the data is in unsigned char, we must map the floats accor-
        dingly. Saturation and value shall be in range [0,255], and hue in 
        range [0,180] (because unsigned char cannot go to 360) => this fol-
        lows the OpenCV-convention for HSV.*/
        m_data[R(i)] = static_cast<unsigned char> (h / 2);
        m_data[G(i)] = static_cast<unsigned char> (s * 255);
        m_data[B(i)] = static_cast<unsigned char> (v * 255);
    }
}

template<ColorSpace C>
void ColorImage<unsigned char, C>::convertFromHsv()
{
    float r, g, b;
    for (int i = 0; i < m_width * m_height; ++i) {
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
        // Hue is undefined here, so we just assign 0.
        *h = 0.0f;
    else {
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
void ColorImage<unsigned char, C>::convertColorFromHsv(float h, float s,
        float v, float* r,
        float* g, float* b)
{
    /* This code is based on Daniel Mohr's version, which again is based on
    the approach presented in Computer Graphics: Principles and
    Practive. Foley et al. */
    /* Given: h in [0, 360) or UNDEFINED, s and v in [0, 1] */
    /* Desired: r, g, b, each in [0, 1]. */
    if (s == 0) {
        if (h == -1.0) {
            *r = v;
            *g = v;
            *b = v;
        }
        else {
            std::cout << "HSV -> RGB conversion error (s == 0 and h > 0)\n" <<
                    std::endl;
            exit(1);
        }
    }
    else {
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

        switch (i) {
            case 0 : 
                *r = v;
                *g = t;
                *b = p;
                break;
            case 1 :
                *r = q;
                *g = v;
                *b = p;
                break;
            case 2 :
                *r = p;
                *g = v;
                *b = t;
                break;
            case 3 :
                *r = p;
                *g = q;
                *b = v;
                break;
            case 4 :
                *r = t;
                *g = p;
                *b = v;
                break;
            case 5 :
                *r = v;
                *g = p;
                *b = q;
                break;
        }
    }
}

template<ColorSpace C >
bool ColorImage< unsigned char, C >::readCv(const std::string& fileName)
{
    // Load image
    cv::Mat cvImage = cv::imread(fileName, cv::IMREAD_UNCHANGED);

    // If image was not loaded (wrong format etc.), then exit
    if (cvImage.empty()) {
        std::cout << "ERROR: OpenCV failed to load file " << fileName << ".\n";
        return false;
    }

    // Check if image has same number of channels as ColorImage instance
    if (cvImage.channels() != m_channels) {
        std::cout << "ERROR: Image does not have compatible number of channels!\n";
        std::cout << "(Expected " << m_channels << " channels, but file has " <<
                cvImage.channels() << " channels.)\n";
        return false;
    }

    // If needed memory for new image exceeds already allocated memory
    int neededMemory = cvImage.size().width * cvImage.size().height *
            cvImage.channels();

    if (neededMemory > m_allocatedMemory) {
        delete[] m_data;
        m_data = new unsigned char[neededMemory];
        m_allocatedMemory = neededMemory;
    }

    m_width = cvImage.size().width;
    m_height = cvImage.size().height;

    // Copy data from cvImage
    unsigned char* data = cvImage.data;

    // Because data in cvImage is not consecutive / "step into next scanline"
    int step = static_cast<int>(cvImage.step);

    // Width in Pixel * Number of Channels
    int numberColumns = cvImage.size().width * cvImage.channels();

    // Goes through all color values of cvImage and copies over values into 
    // array. No scale is applied because of unsigned char specialization.
    int pixelNo = 0;
    for (int i = 0; i < cvImage.size().height; ++i) {
        for (int j = 0; j < numberColumns; j += cvImage.channels()) {

            if (cvImage.channels() == 1) // e.g. grey-scale image
            {
                m_data[pixelNo] = data[j];
                ++pixelNo;
            }
            else if (cvImage.channels() >= 3) // e.g. RGB
            {
                m_data[B(pixelNo)] = data[j];
                m_data[G(pixelNo)] = data[j + 1];
                m_data[R(pixelNo)] = data[j + 2];
                if (cvImage.channels() == 4) // e.g. RGBA
                {
                    m_data[A(pixelNo)] = data[j + 3];
                }
                ++pixelNo;
            }
        }
        data += step;
    }
    return true;
}

template<ColorSpace C >
void ColorImage< unsigned char, C >::setRGBAOffsets()
{
    switch (C) {
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

template<ColorSpace C>
int ColorImage< float, C >::getNumberOfChannels(
        const ColorSpace colorSpace)
{
    switch (colorSpace) {
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



// ------
// PUBLIC
// ------



// Constructors

template<ColorSpace C >
ColorImage<float, C >::ColorImage() : 
    Image(),
    m_channels(getNumberOfChannels(C))
{
    setRGBAOffsets();
    std::cout << "ColorImage default constructor" << std::endl;
}

template<ColorSpace C>
ColorImage<float, C >::ColorImage(int width, int height) :
    Image(width, height),
    m_channels(getNumberOfChannels(C))
{
    setRGBAOffsets();
    m_allocatedMemory = m_width * m_height * m_channels;
    m_data = m_allocatedMemory ? new float[m_allocatedMemory] : nullptr;
    std::cout << "ColorImage width/height constructor" << std::endl;
}

template<ColorSpace C >
ColorImage<float, C >::ColorImage(const std::string& fileName) :
    Image(),
    m_channels(getNumberOfChannels(C))
{
    setRGBAOffsets();
    // allocateMemory() call later in readCv, since width and height have to be
    // read from file first.
    read(fileName);
    std::cout << "ColorImage filename constructor" << std::endl;
}

template<ColorSpace C >
ColorImage<float, C >::ColorImage(const ColorImage& other) :
    Image(other),
    m_channels(other.m_channels),
    m_offsetR(other.m_offsetR), m_offsetG(other.m_offsetG),
    m_offsetB(other.m_offsetB), m_offsetA(other.m_offsetA)
{
}


// UNDONE: Move Constructor
//template<ColorSpace C>
//ColorImage<unsigned char, C>::ColorImage(ColorImage&& other) :
//    Image(other),
//    m_channels(other.m_channels),
//    m_offsetR(other.m_offsetR), m_offsetG(other.m_offsetG), 
//    m_offsetB(other.m_offsetB), m_offsetA(other.m_offsetA)
//{
//    // Do not reset other here, values like offset and channels are tied to template.
//}

// Destructor
template<ColorSpace C >
ColorImage<float, C >::~ColorImage()
{
    std::cout << "ColorImage destructor" << std::endl;
}

template<ColorSpace C >
ColorImage<float, C>& ColorImage<float, C>::operator=(const ColorImage& rhs)
{
    if (this != &rhs) {
        // No need for deleting current offsets, will stay the same anyway, tied to template.
        Image::operator=(rhs);
    }
    return *this;
}


// UNDONE: Move Assignment Operator
//template<ColorSpace C>
//ColorImage<unsigned char, C>& ColorImage<unsigned char, C>::operator =(
//    ColorImage&& rhs)
//{
//    if (this != &rhs)
//    {
//        // No need for deleting current offsets, will stay the same anyway, tied to template.
//        Image::operator =(rhs);
//    }
//    return *this;
//}

template<ColorSpace C >
bool ColorImage<float, C >::read(const std::string& fileName)
{
    // Create dummy file and check if it was a success before trying to 
    // read image.
    std::ifstream dummy;
    // Opening dummy file to check if it exists at all.
    // Replaced the old C-directives (may be slower).
    dummy.open(fileName);

    bool throwsError = false;
    if (!dummy.is_open()) {
        std::cout << "ERROR: Could not open or find file " << fileName << ".\n";
        throwsError = true;
    }

    if (!throwsError) {
        std::cout << "Trying to open file with OpenCV..." << std::endl;
        // Actual Reading via OpenCV
        throwsError = readCv(fileName);
    }

    // UNDONE: What happens if C == CS_LAB?
    if (C == ColorSpace::CS_HSV) {
        ColorImage::convertToHsv();
    }

    return throwsError;
}

template<ColorSpace C >
void ColorImage<float, C >::write(const std::string& fileName) const
{
    int numValues = m_width * m_height * m_channels;
    float* data = new float[numValues];

    for (int i = 0; i < numValues; i += m_channels) {
        data[i] = m_data[i + m_offsetB] * 255.0f;            // Works for gray (since offset = 0)
        if (m_channels >= 3) {                      
            data[i + 1] = m_data[i + m_offsetG] * 255.0f;
            data[i + 2] = m_data[i + m_offsetR] * 255.0f;
        }
        if (m_channels == 4) {
            data[i + 3] = m_data[i + m_offsetA] * 255.0f;
        }
    }

    switch (C) {
        case ColorSpace::CS_RGB:
        case ColorSpace::CS_BGR:
        case ColorSpace::CS_HSV:
            cv::imwrite(fileName, cv::Mat(m_height, m_width, CV_32FC3, data));
            break;
        case ColorSpace::CS_RGBA:
        case ColorSpace::CS_BGRA:
        case ColorSpace::CS_ARGB:
            cv::imwrite(fileName, cv::Mat(m_height, m_width, CV_32FC4, data));
            break;
        case ColorSpace::CS_GRAY:
            cv::imwrite(fileName, cv::Mat(m_height, m_width, CV_32FC1, data));
            break;
    }

    delete[] data;
}

template<ColorSpace C >
void ColorImage<float, C >::copy(ColorImage* output) const
{
    // First compare width and height, than copy if the same.
    if (compareDimensions(*output)) {
        memcpy(output->m_data, m_data, m_allocatedMemory * sizeof (float));
    }
}

// TODO: Method is massive, cut size down somehow.
template<ColorSpace C > template<ColorSpace D>
void ColorImage< float, C >::convertColorSpace( 
    ColorImage< float, D >* output) const
{
    output->m_width = m_width;
    output->m_height = m_height;

    // Allocate new memory if old data array is too small
    int neededMemory = m_width * m_height * output->m_channels;

    if (neededMemory > output->m_allocatedMemory) {
        delete[] output->m_data;
        output->m_data = new float[neededMemory];
        output->m_allocatedMemory = neededMemory;
    }

    if (C == D) {
        memcpy(output->m_data, m_data, neededMemory * sizeof (float));
    }
    else {
        // Case 1: Both images have same number of channels
        if (m_channels == output->m_channels) { 
            for (int i = 0; i < neededMemory; ++i) {
                // Simple memcpy() does not work here since channel order can be
                // different.
                output->m_data[i + output->m_offsetB] = m_data[i + m_offsetB];
                if (output->m_channels >= 3) {
                    output->m_data[i + output->m_offsetG] = m_data[i + m_offsetG];
                    output->m_data[i + output->m_offsetR] = m_data[i + m_offsetR];
                }
                if (output->m_channels == 4) {
                    output->m_data[i + output->m_offsetA] = m_data[i + m_offsetA];
                }
            }
        }
        // Case 2: source image has more channels
        else if (m_channels > output->m_channels) {
            // if output is grey image, convert.
            if (m_channels >= 3 && output->m_channels == 1) {
                int pixelOffset = 0;
                float averageColor = 0;

                for (int i = 0; i < neededMemory; ++i) {
                    // Calculate average color value and assign it.
                    // TODO: Other weights (luminosity etc.) instead of just average color.
                    averageColor = (m_data[pixelOffset + m_offsetB] +
                            m_data[pixelOffset + m_offsetG] +
                            m_data[pixelOffset + m_offsetR]) / 3.0f;
                    output->m_data[i] = averageColor;
                    pixelOffset += m_channels;
                }
            } 
            // If source has 4 channels and output only 3, strip off alpha.
            else if (m_channels == 4 && output->m_channels == 3) {
                int pixelOffset = 0;
                for (int i = 0; i < neededMemory; i += output->m_channels) {
                    output->m_data[i + output->m_offsetB] =
                            m_data[pixelOffset + m_offsetB];
                    output->m_data[i + output->m_offsetG] =
                            m_data[pixelOffset + m_offsetG];
                    output->m_data[i + output->m_offsetR] =
                            m_data[pixelOffset + m_offsetR];

                    pixelOffset += m_channels;
                }
            }
        }
        // Case 3: source image has less channels than output image
        else if (m_channels < output->m_channels) { 
            int pixelOffset = 0;
            for (int i = 0; i < neededMemory; i += output->m_channels) {
                output->m_data[i + output->m_offsetB] = m_data[pixelOffset + m_offsetB];
                output->m_data[i + output->m_offsetG] = m_data[pixelOffset + m_offsetG];
                output->m_data[i + output->m_offsetR] = m_data[pixelOffset + m_offsetR];
                
                // if output image has alpha channel
                if (output->m_channels == 4) {
                    output->m_data[i + output->m_offsetA] = 1.0f;
                }

                pixelOffset += m_channels;
            }
        }
    }

    // If needed, convert from/to HSV based system.
    if (C == ColorSpace::CS_HSV && D != ColorSpace::CS_HSV) {
        output->convertFromHsv();
    }
    else if (C != ColorSpace::CS_HSV && D == ColorSpace::CS_HSV) {
        output->convertToHsv();
    }
}

template<ColorSpace C>
void ColorImage<float, C>::convertType(
        ColorImage<unsigned char, C>* output) const
{
    output->m_width = m_width;
    output->m_height = m_height;
    output->m_allocatedMemory = m_allocatedMemory;

    delete[] output->m_data;
    output->m_data = new unsigned char[m_allocatedMemory];

    for (int i = 0; i < m_allocatedMemory; ++i) {
        output->m_data[i] = static_cast<unsigned char>(m_data[i] * 255.0f);
    }
}

template<ColorSpace C > template<typename U, ColorSpace D >
bool ColorImage<float, C >::compareDimensions(
        const ColorImage< U, D >& other) const
{
    bool sameDimensions = false;
    if (m_width == other.getWidth()
            && m_height == other.getHeight()) {
        sameDimensions = true;
    }
    return sameDimensions;
}

template<ColorSpace C >
int ColorImage<float, C >::R(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetR;
}

template<ColorSpace C >
int ColorImage<float, C >::R(int x, int y) const
{
    return R((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage<float, C >::G(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetG;
}

template<ColorSpace C >
int ColorImage<float, C >::G(int x, int y) const
{
    return G((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage<float, C >::B(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetB;
}

template<ColorSpace C >
int ColorImage<float, C >::B(int x, int y) const
{
    return B((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage<float, C >::A(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetA;
}

template<ColorSpace C >
int ColorImage<float, C >::A(int x, int y) const
{
    return A((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage<float, C >::H(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetR;
}

template<ColorSpace C >
int ColorImage<float, C >::H(int x, int y) const
{
    return H((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage<float, C >::S(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetG;
}

template<ColorSpace C >
int ColorImage<float, C >::S(int x, int y) const
{
    return S((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage<float, C >::V(int pixelNo) const
{
    return pixelNo * m_channels + m_offsetB;
}

template<ColorSpace C >
int ColorImage<float, C >::V(int x, int y) const
{
    return V((y * m_width) + x);
}

template<ColorSpace C >
int ColorImage<float, C >::getChannels() const
{
    return m_channels;
}

template<ColorSpace C >
unsigned int ColorImage<float, C >::getOffsetR() const
{
    return m_offsetR;
}

template<ColorSpace C >
unsigned int ColorImage<float, C >::getOffsetG() const
{
    return m_offsetG;
}

template<ColorSpace C >
unsigned int ColorImage<float, C >::getOffsetB() const
{
    return m_offsetB;
}

template<ColorSpace C >
unsigned int ColorImage<float, C >::getOffsetA() const
{
    return m_offsetA;
}



// -------
// PRIVATE
// -------

template<ColorSpace C>
void ColorImage<float, C>::convertToHsv()
{
    float h, s, v;
    for (int i = 0; i < m_width * m_height; ++i) {
        convertColorToHsv(m_data[R(i)], m_data[G(i)], m_data[B(i)], &h, &s, &v);

        m_data[R(i)] = h / 360.0f;  // Normalizing the hue value.
        m_data[G(i)] = s;
        m_data[B(i)] = v;
    }
}

template<ColorSpace C>
void ColorImage<float, C>::convertFromHsv()
{
    float r, g, b;
    for (int i = 0; i < m_width * m_height; ++i) {
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
    if (max != 0.0f) {
        *s = (max - min) / max;
    }
    else {
        *s = 0.0f;
    }

    // Calculate hue value
    if (*s == 0.0f) {
        // Hue is undefined here, so we just assign 0.
        *h = 0.0f;
    }
    else {
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
void ColorImage<float, C>::convertColorFromHsv(float  h, float  s, float  v,
                                               float* r, float* g, float* b)
{
    /* This code is based on Daniel Mohr's version, which again is based on
    the approach presented in Computer Graphics: Principles and
    Practive. Foley et al. */
    /* Given: h in [0, 360) or UNDEFINED, s and v in [0, 1] */
    /* Desired: r, g, b, each in [0, 1]. */
    if (s == 0) {
        if (h == -1.0) {
            *r = v;
            *g = v;
            *b = v;
        }
        else {
            std::cout << "HSV -> RGB conversion error (s == 0 and h > 0)\n" <<
                    std::endl;
            exit(1);
        }
    }
    else {
        float f, p, q, t;
        int i;

        if (h == 360.0) {
            h = 0.0;
        }

        h /= 60;
        i = static_cast<int> (std::floor(h));
        f = h - i;
        p = v * (1.0f - s);
        q = v * (1.0f - (s * f));
        t = v * (1.0f - (s * (1.0f - f)));

        switch (i) {
            case 0 : 
                *r = v;
                *g = t;
                *b = p;
                break;
            case 1 :
                *r = q;
                *g = v;
                *b = p;
                break;
            case 2 :
                *r = p;
                *g = v;
                *b = t;
                break;
            case 3 :
                *r = p;
                *g = q;
                *b = v;
                break;
            case 4 :
                *r = t;
                *g = p;
                *b = v;
                break;
            case 5 :
                *r = v;
                *g = p;
                *b = q;
                break;
        }
    }
}

template<ColorSpace C >
bool ColorImage<float, C >::readCv(const std::string& fileName)
{
    // Load image
    cv::Mat cvImage = cv::imread(fileName, cv::IMREAD_UNCHANGED);

    // If image was not loaded (wrong format etc.), then exit
    if (cvImage.empty()) {
        std::cout << "ERROR: OpenCV failed to load file " << fileName << ".\n";
        return false;
    }

    // Check if image has same number of channels as ColorImage instance
    if (cvImage.channels() != m_channels) {
        std::cout << "ERROR: Image does not have compatible number of channels!\n";
        std::cout << "(Expected " << m_channels << " channels, but file has " <<
                cvImage.channels() << " channels.)\n";
        return false;
    }

    // If needed memory for new image exceeds already allocated memory
    int neededMemory = cvImage.size().width * 
                       cvImage.size().height *
                       cvImage.channels();

    if (neededMemory > m_allocatedMemory) {
        delete[] m_data;
        m_data = new float[neededMemory];
        m_allocatedMemory = neededMemory;
    }

    m_width  = cvImage.size().width;
    m_height = cvImage.size().height;

    // Copy data from cvImage
    unsigned char* data = cvImage.data;

    // Because data in cvImage is not consecutive / "step into next scanline"
    int step = static_cast<int>(cvImage.step);

    // Width in Pixel * Number of Channels
    int numberColumns = cvImage.size().width * cvImage.channels();

    // Goes through all color values of cvImage and copies over values into 
    // array. No scale is applied because of unsigned char specialization.
    int pixelNo = 0;
    for (int i = 0; i < cvImage.size().height; ++i) {
        for (int j = 0; j < numberColumns; j += cvImage.channels()) {

            if (cvImage.channels() == 1) {          // e.g. grey-scale image
                m_data[pixelNo] = data[j] / 255.0f;

                ++pixelNo;
            }
            else if (cvImage.channels() >= 3) {     // e.g. RGB
                m_data[B(pixelNo)] = data[j] / 255.0f;
                m_data[G(pixelNo)] = data[j + 1] / 255.0f;
                m_data[R(pixelNo)] = data[j + 2] / 255.0f;

                if (cvImage.channels() == 4) {        // e.g. RGBA
                    m_data[A(pixelNo)] = data[j + 3] / 255.0f;
                }

                ++pixelNo;
            }
        }
        data += step;
    }
    return true;
}

template<ColorSpace C >
void ColorImage<float, C >::setRGBAOffsets()
{
    switch (C) {
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