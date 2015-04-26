/*
 *	This file must be excluded from build!
 */


//==============================
// Public Member Functions
//==============================

template <class T, ColorSpace C>
ColorImage<T, C>::ColorImage() : m_channels(getNumChannels())
{
    setRGBAOffsets();
    std::clog << "ColorImage default constructor" << std::endl;
}


template <class T, ColorSpace C>
ColorImage<T, C>::ColorImage(int width, int height)
    : Image(width, height),
      m_channels(getNumChannels())
{
    std::clog << "ColorImage width/height constructor" << std::endl;
    setRGBAOffsets();
    m_data.resize(size() * m_channels);
}


template <class T, ColorSpace C>
ColorImage<T, C>::ColorImage(const std::string& fileName)
    : m_channels(getNumChannels())
{
    std::clog << "ColorImage filename constructor" << std::endl;
    setRGBAOffsets();
    read(fileName);
}


template <class T, ColorSpace C>
ColorImage<T, C>::~ColorImage()
{
    std::clog << "ColorImage destructor" << std::endl;
}


template <class T, ColorSpace C>
int ColorImage<T, C>::getNumChannels()
{
    switch (C)
    {
    case ColorSpace::RGB:
    case ColorSpace::BGR:
    case ColorSpace::HSV:
    case ColorSpace::LAB:
        return 3;
    case ColorSpace::RGBA:
    case ColorSpace::BGRA:
    case ColorSpace::ARGB:
        return 4;
    case ColorSpace::GRAY:
        return 1;
    default:
        return 0;
    }
}


template <class T, ColorSpace C>
int ColorImage<T, C>::getCvType()
{
    switch (C)
    {
    case ColorSpace::RGB:
    case ColorSpace::BGR:
    case ColorSpace::HSV:
    case ColorSpace::LAB:
        return std::is_same<T, unsigned char>::value ? CV_8UC3 : CV_32FC3;
    case ColorSpace::RGBA:
    case ColorSpace::BGRA:
    case ColorSpace::ARGB:
        return std::is_same<T, unsigned char>::value ? CV_8UC4 : CV_32FC4;
    case ColorSpace::GRAY:
        return std::is_same<T, unsigned char>::value ? CV_8UC1 : CV_32FC1;
    default:
        return 0;
    }
}

template <class T, ColorSpace C>
bool ColorImage<T, C>::read(const std::string& fileName)
{
    // Create dummy file and check if it was a success before trying to 
    // read image.
    std::ifstream dummy;
    dummy.open(fileName);

    bool throwsError;
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
        if (!throwsError && C == ColorSpace::HSV)
        {
            convertToHsv();
        }
    }
    return throwsError;
}


template <class T, ColorSpace C>
void ColorImage<T, C>::write(const std::string& fileName) const
{
    if (m_width == 0 && m_height == 0)
    {
        std::cerr << "Image cannot be written to " << fileName << ", because it is empty!" << std::endl;
        return;
    }

    std::vector<T> data(size() * m_channels);

    auto factor = getColorValueFactor(T(), unsigned char());
    // Convert to BGRA-space for OpenCV.
    for (int i = 0; i < size(); ++i)
    {
        // For 1 channel outside if, because grey-channel values
        // are the same in each channel; does not matter which to copy.
        data[(i * m_channels)] = m_data[B(i)] * factor;
        if (m_channels >= 3)
        {
            data[(i * m_channels) + 1] = m_data[G(i)] * factor;
            data[(i * m_channels) + 2] = m_data[R(i)] * factor;
        }
        if (m_channels == 4)
            data[(i * m_channels) + 3] = m_data[A(i)] * factor;
    }

    cv::Mat output(m_height, m_width, getCvType(), data.data());
    cv::imwrite(fileName, output);
}


template <class T, ColorSpace C>
void ColorImage<T, C>::copyTo(ColorImage* output) const
{
    output->m_width = m_width;
    output->m_height = m_height;
    output->m_data = m_data;
}


template <class T, ColorSpace C>
template <ColorSpace D>
void ColorImage<T, C>::convertColorSpace(ColorImage<T, D>* output) const
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
        // if both images have alpha
        if (m_channels == 4 && output->m_channels == 4)
        {
            // just copy over the color values in the new order
            output->setPixelColor(i, 
                                  m_data[R(i)],
                                  m_data[G(i)],
                                  m_data[B(i)],
                                  m_data[A(i)]);
        }
        // Conversion to grey image.
        // input.m_channels is implicitly greater than output's
        else if (output->m_channels == 1)
        {
            // Calculate average color value and assign it.
            // TODO: Other weights (luminosity etc.) instead of just average color.
            auto averageColor = (m_data[B(i)] + m_data[G(i)] + m_data[R(i)]) / 3.0f;
            auto color = static_cast<T>(averageColor);
            output->setPixelColor(i,
                                  color,
                                  color,
                                  color);
        }
        // else copy color values in new order, but omit alpha or set to default value
        else
        {
            output->setPixelColor(i, m_data[R(i)], m_data[G(i)], m_data[B(i)]);
        }
    }

    // If needed, convert from/to HSV-/Lab-space.
    // UNDONE: Case HSV <-> Lab
    if (C == ColorSpace::HSV && D != ColorSpace::HSV)
        output->convertFromHsv();
    else if (C != ColorSpace::HSV && D == ColorSpace::HSV)
        output->convertToHsv();
    //else if (C == ColorSpace::LAB && D != ColorSpace::LAB)
    //    output->convertFromLab();
    //else if (C != ColorSpace::LAB && D == ColorSpace::LAB)
    //     output->convertToLab();
}


template <class T, ColorSpace C>
template <class U>
void ColorImage<T, C>::convertType(ColorImage<U, C>* output) const
{
    output->resize(m_width, m_height);

    auto factor = getColorValueFactor(T(), U());
    for (int i = 0; i < output->m_data.size(); ++i)
        output->m_data[i] = static_cast<U>(m_data[i] * factor);
}


template <class T, ColorSpace C>
void ColorImage<T, C>::resize(int width, int height)
{
    m_width = width;
    m_height = height;
    m_data.resize(m_width * m_height * m_channels);
}


template <class T, ColorSpace C>
template <class U, ColorSpace D>
bool ColorImage<T, C>::sameSize( const ColorImage<U, D>& other) const
{
    if (m_width == other.getWidth() && m_height == other.getHeight())
        return true;

    return false;
}



template <class T, ColorSpace C>
void ColorImage<T, C>::setPixelColor(int pixelIdx, 
                                        T r,
                                        T g,
                                        T b,
                                        T a)
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


template <class T, ColorSpace C>
void ColorImage<T, C>::setToBlack()
{
    setToColor(0, 0, 0);
}


template <class T, ColorSpace C>
void ColorImage<T, C>::setToValue(T value)
{
    setToColor(value, value, value);
}


template <class T, ColorSpace C>
void ColorImage<T, C>::setToColor(T r, T g, T b)
{
    auto alpha = std::is_same<T, unsigned char>::value ? 255 : 1.0; // This is more or less hardcoded, but no partial specialization needed.
    for (unsigned i = 0; i < size(); ++i)
        setPixelColor(i, r, g, b, alpha);
}


template <class T, ColorSpace C>
void ColorImage<T, C>::setToColor(T r, T g, T b, T a)
{
    for (unsigned i = 0; i < size(); ++i)
        setPixelColor(i, r, g, b, a);
}


template <class T, ColorSpace C>
int ColorImage<T, C>::R(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetR;
}


template <class T, ColorSpace C>
int ColorImage<T, C>::R(int x, int y) const
{
    return R((y * m_width) + x);
}


template <class T, ColorSpace C>
int ColorImage<T, C>::G(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetG;
}


template <class T, ColorSpace C>
int ColorImage<T, C>::G(int x, int y) const
{
    return G((y * m_width) + x);
}


template <class T, ColorSpace C>
int ColorImage<T, C>::B(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetB;
}


template <class T, ColorSpace C>
int ColorImage<T, C>::B(int x, int y) const
{
    return B((y * m_width) + x);
}


template <class T, ColorSpace C>
int ColorImage<T, C>::A(int pixelIdx) const
{
    if (m_offsetA == -1)
        return -1;
    else
        return pixelIdx * m_channels + m_offsetA;
}


template <class T, ColorSpace C>
int ColorImage<T, C>::A(int x, int y) const
{
    return A((y * m_width) + x);
}


template <class T, ColorSpace C>
int ColorImage<T, C>::H(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetR;
}


template <class T, ColorSpace C>
int ColorImage<T, C>::H(int x, int y) const
{
    return H((y * m_width) + x);
}


template <class T, ColorSpace C>
int ColorImage<T, C>::S(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetG;
}


template <class T, ColorSpace C>
int ColorImage<T, C>::S(int x, int y) const
{
    return S((y * m_width) + x);
}


template <class T, ColorSpace C>
int ColorImage<T, C>::V(int pixelIdx) const
{
    return pixelIdx * m_channels + m_offsetB;
}


template <class T, ColorSpace C>
int ColorImage<T, C>::V(int x, int y) const
{
    return V((y * m_width) + x);
}


template <class T, ColorSpace C>
int ColorImage<T, C>::getChannels() const
{
    return m_channels;
}


template <class T, ColorSpace C>
unsigned ColorImage<T, C>::getOffsetR() const
{
    return m_offsetR;
}


template <class T, ColorSpace C>
unsigned ColorImage<T, C>::getOffsetG() const
{
    return m_offsetG;
}


template <class T, ColorSpace C>
unsigned ColorImage<T, C>::getOffsetB() const
{
    return m_offsetB;
}


template <class T, ColorSpace C>
unsigned ColorImage<T, C>::getOffsetA() const
{
    return m_offsetA;
}

// =======================================================================================
// Private Member Functions
// ======================================================================================= 

template <class T, ColorSpace C>
void ColorImage<T, C>::convertToHsv()
{
    float h, s, v;
    auto factor = getColorValueFactor(T(), float());
    auto factorInv = getColorValueFactor(float(), T());
    for (int i = 0; i < size(); ++i)
    {
        convertColorToHsv(m_data[R(i)] * factor, // Since conversion-function
                          m_data[G(i)] * factor, // takes floats, we must di-
                          m_data[B(i)] * factor, // vide by 255 if needed.
                          &h, &s, &v);

        // saturation and value shall be in range [0,255] / [0, 1], and hue in
        // range [0,180] / [0, 360] (because unsigned char cannot go to 360)
        // this follows the OpenCV-convention for HSV.
        m_data[R(i)] = static_cast<T>(h / 2);
        m_data[G(i)] = static_cast<T>(s * factorInv);
        m_data[B(i)] = static_cast<T>(v * factorInv);
    }
}


template <class T, ColorSpace C>
void ColorImage<T, C>::convertFromHsv()
{
    float r, g, b;
    auto hueScale = std::is_same<T, unsigned char>::value ? 2 : 1.0; // This is more or less hardcoded, but no partial specialization needed.
    auto factor = getColorValueFactor(T(), float());
    auto factorInv = getColorValueFactor(float(), T());

    for (unsigned i = 0; i < m_width * m_height; ++i)
    {
        // Conversion function expects hue in range [0,360] and s,v [0,1].
        convertColorFromHsv(m_data[H(i)] * hueScale,
                            m_data[S(i)] * factor,
                            m_data[V(i)] * factor,
                            &r, &g, &b);

        // Since the data is in unsigned char, we must map the floats accordingly.
        m_data[R(i)] = static_cast<T> (r * factorInv);
        m_data[G(i)] = static_cast<T> (g * factorInv);
        m_data[B(i)] = static_cast<T> (b * factorInv);
    }
}


template <class T, ColorSpace C>
void ColorImage<T, C>::convertColorToHsv(float  r,
                                        float  g,
                                        float  b,
                                        float* h,
                                        float* s,
                                        float* v)
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


template <class T, ColorSpace C>
void ColorImage<T, C>::convertColorFromHsv(float h,
                                           float s,
                                           float v,
                                           float* r,
                                           float* g,
                                           float* b)
{
    /* This code is based on Daniel Mohr's version, which again is based on
     * the approach presented in Computer Graphics: Principles and
     * Practive. Foley et al. */
    /* Given: h in [0, 360) or UNDEFINED, s and v in [0, 1] */
    /* Desired: r, g, b, each in [0, 1]. */
    if (s == 0)
    {
        // achromatic (grey)
        *r = *g = *b = v;
        return;
    }

    float f, p, q, t;
    int i;

    if (h == 360.0)
    {
        h = 0.0;
    }

    h /= 60;
    i = static_cast<int>(std::floor(h));
    f = h - i;
    p = v * (1.0f - s);
    q = v * (1.0f - (s * f));
    t = v * (1.0f - (s * (1.0f - f)));

    switch (i)
    {
    case 0:
        *r = v; *g = t; *b = p;
        break;
    case 1:
        *r = q; *g = v; *b = p;
        break;
    case 2:
        *r = p; *g = v; *b = t;
        break;
    case 3:
        *r = p; *g = q; *b = v;
        break;
    case 4:
        *r = t; *g = p; *b = v;
        break;
    default:       // case 5 :
        *r = v; *g = p; *b = q;
        break;
    }
}


//// Based on Daniel Mohr's implementation.
//template <class T, ColorSpace C>
//void ColorImage<T, C>::convertToLab()
//{
//    float lab[3];
//    float min[3] = { std::numeric_limits<float>::max() };
//    float max[3] = { -std::numeric_limits<float>::max() };
//
//    int numPixels = m_width * m_height;
//    for (int i = 0; i < numPixels; ++i)
//    {
//        convertColorToLab(m_data[R(i)],
//                          m_data[G(i)],
//                          m_data[B(i)],
//                          &lab[0],
//                          &lab[1],
//                          &lab[2]);
//
//        m_data[R(i)] = lab[0];
//        m_data[G(i)] = lab[1];
//        m_data[B(i)] = lab[2];
//    }
//}
//
//
//// Based on Daniel Mohr's implementation.
//template <class T, ColorSpace C>
//void ColorImage<T, C>::convertFromLab()
//{
//    float r, g, b;
//    for (unsigned i = 0; i < size(); ++i)
//    {
//        convertColorFromLab(m_data[R(i)],
//                            m_data[G(i)],
//                            m_data[B(i)],
//                            &r, &g, &b);
//
//        // Since the data is in unsigned char, we must map the floats
//        // accordingly.
//        m_data[R(i)] = r;
//        m_data[G(i)] = g;
//        m_data[B(i)] = b;
//    }
//}
//
//// Based on Daniel Mohr's / Wikipedia's implementation.
//template <class T, ColorSpace C>
//void ColorImage<T, C>::convertColorToLab(float r, float g, float b,
//                                             float* lo, float* ao, float* bo)
//{
//    float x, y, z;
//    // Converts r, g, b to x, y, z
//    x = (0.412453f * r) + (0.357580f * g) + (0.180423f * b);
//    y = (0.212671f * r) + (0.715160f * g) + (0.072169f * b);
//    z = (0.019334f * r) + (0.119193f * g) + (0.950227f * b);
//
//    // Reference white points.
//    const float X_WHITE = 1.0f / 3.0f;
//    const float Y_WHITE = 1.0f / 3.0f;
//    const float Z_WHITE = 1.0f / 3.0f;
//
//    x = x / X_WHITE;
//    y = y / Y_WHITE;
//    z = z / Z_WHITE;
//
//    bool xr = x > 0.0088564517f;
//    bool yr = y > 0.0088564517f;
//    bool zr = z > 0.0088564517f;
//
//    float fX, fY, fZ;
//    fX = xr ? std::pow(x, (1.0f / 3.0f)) : (7.787f * x + (4.0f / 29.0f));
//    fY = yr ? std::pow(y, (1.0f / 3.0f)) : (7.787f * y + (4.0f / 29.0f));
//    fZ = zr ? std::pow(z, (1.0f / 3.0f)) : (7.787f * z + (4.0f / 29.0f));
//
//    *lo = 116.0f * fY - 16.0f;
//    *ao = 500.0f * (fX - fY);
//    *bo = 200.0f * (fY - fZ);
//}
//
//// Based on Daniel Mohr's implementation.
//template <class T, ColorSpace C>
//void ColorImage<T, C>::convertColorFromLab(float lIn, float aIn, float bIn,
//                                               float* r, float* g, float* b)
//{
//    // Reference white points.
//    const float X_WHITE = 1.0f / 3.0f;
//    const float Y_WHITE = 1.0f / 3.0f;
//    const float Z_WHITE = 1.0f / 3.0f;
//
//    float x, y, z;
//    y = (1.0f / 116.0f) * (lIn + 16.0f);
//    x = (1.0f / 116.0f) * (lIn + 16.0f) + (1.0f / 500.0f) * aIn;
//    z = (1.0f / 116.0f) * (lIn + 16.0f) - (1.0f / 200.0f) * bIn;
//
//    bool xr = x > (6.0f / 29.0f);
//    bool yr = y > (6.0f / 29.0f);
//    bool zr = z > (6.0f / 29.0f);
//
//    // Use inverse from function f from forward conversion.
//    float fInverseX = xr ?
//        std::pow(x, 3.0f) : ((108.0f / 841.0f) * (x - (4.0f / 29.0f)));
//    float fInverseY = yr ?
//        std::pow(y, 3.0f) : ((108.0f / 841.0f) * (y - (4.0f / 29.0f)));
//    float fInverseZ = zr ?
//        std::pow(z, 3.0f) : ((108.0f / 841.0f) * (y - (4.0f / 29.0f)));
//
//    x = X_WHITE * fInverseX;
//    y = Y_WHITE * fInverseY;
//    z = Z_WHITE * fInverseZ;
//
//    // Converts x, y, z into r, g, b.
//    *r = ( 3.240479f * x) + (-1.53715f  * y) + (-0.498535f * z);
//    *g = (-0.969256f * x) + ( 1.875991f * y) + ( 0.041556f * z);
//    *b = ( 0.055648f * x) + (-0.204043f * y) + ( 1.057311f * z);
//}


template <class T, ColorSpace C>
bool ColorImage<T, C>::readCv(const std::string& fileName)
{
    auto cvImage = cv::imread(fileName, cv::IMREAD_UNCHANGED);
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

    m_width = cvImage.size().width;
    m_height = cvImage.size().height;
    m_data.resize(m_width * m_height * m_channels);

    auto data = cvImage.data;

    auto factor = getColorValueFactor(unsigned char(), T());
    for (int i = 0; i < size(); ++i)
    {
        // BUG: Not actually a bug, but unnecessary: we pass some values
        // as parameters, even though they might not be used (e.g. alpha value
        // in 3-channel-image.
        setPixelColor(i, data[2] * factor, data[1] * factor, data[0] * factor, data[3] * factor);	// in this order because cvIMage is BGR(A) space
        data += m_channels;
    }

    return true;
}


template <class T, ColorSpace C>
void ColorImage<T, C>::setRGBAOffsets()
{
    switch (C)
    {
    case ColorSpace::RGB:
    case ColorSpace::HSV:
    case ColorSpace::LAB:
        m_offsetR = 0;
        m_offsetG = 1;
        m_offsetB = 2;
        m_offsetA = -1;
        break;
    case ColorSpace::GRAY:
        m_offsetR = m_offsetG = m_offsetB = m_offsetA = 0;
        break;
    case ColorSpace::BGR:
        m_offsetR = 2;
        m_offsetG = 1;
        m_offsetB = 0;
        m_offsetA = -1;
        break;
    case ColorSpace::RGBA:
        m_offsetR = 0;
        m_offsetG = 1;
        m_offsetB = 2;
        m_offsetA = 3;
        break;
    case ColorSpace::BGRA:
        m_offsetR = 2;
        m_offsetG = 1;
        m_offsetB = 0;
        m_offsetA = 3;
        break;
    case ColorSpace::ARGB:
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