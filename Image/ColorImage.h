#define _CRT_SECURE_NO_WARNINGS		//TODO To be able to use the fopen() function

#pragma once

#include "Image.h"

#include<iostream>
#include<string>
#include<fstream>		// For dummy file reading
#include<algorithm>	// For std::max()
#include<type_traits>

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>

/**
*	Defines various color spaces.
*	Declared outside of class because of template.
**/
enum class ColorSpace
{
    CS_GRAY = 0, CS_RGB, CS_BGR, CS_HSV, CS_LAB, CS_RGBA, CS_BGRA, CS_ARGB
};

template < typename T, ColorSpace C >
class ColorImageBase : public Image < T >
{

};

template < typename T, ColorSpace C >
class ColorImage : public Image < T >
{
    //public:
    //	// Implementing each constructor because C++11 (which features constructor delegation) is only partially implemented in VS2012.
    //
    //	ColorImage();
    //	ColorImage(int width, int height);
    //	explicit ColorImage(const std::string& fileName);
    //
    //	~ColorImage();
    //
    //	static int getNumberOfChannels(const ColorSpace colorSpace);
    //
    //	bool read(const std::string& fileName);
    //	void write(const std::string& fileName) const;
    //	void copy(ColorImage* outputImage) const;
    //
    //	template<typename U, ColorSpace OtherClrSpace>
    //	void convert(ColorImage<U, OtherClrSpace>* out) const;
    //
    //	// Accessors
    //
    //	// Returns the (n + 1) red-value-index from the image's data
    //	int R(int n) const { return n * m_channels + m_offsetR; }
    //	// Returns the (n + 1) green-value-index from the image's data
    //	int G(int n) const { return n * m_channels + m_offsetG; }
    //	// Returns the (n + 1) blue-value-index from the image's data
    //	int B(int n) const { return n * m_channels + m_offsetB; }
    //	// Returns the (n + 1) alpha-value-index from the image's data
    //	int A(int n) const { return n * m_channels + m_offsetA; }
    //	// Returns the (n + 1) hue-value-index (red-value-index) from the image's data
    //	int H(int n) const { return n * m_channels + m_offsetR; }
    //	// Returns the (n + 1) saturation-value-index (green-value-index) from the image's data
    //	int S(int n) const { return n * m_channels + m_offsetG; }
    //	// Returns the (n + 1) value-value-index (blue-value-index) from the image's data
    //	int V(int n) const { return n * m_channels + m_offsetB; }
    //
    //	// Getters
    //	int getChannels() const;
    //	unsigned int getOffsetR() const;
    //	unsigned int getOffsetG() const;
    //	unsigned int getOffsetB() const;
    //	unsigned int getOffsetA() const;
    //	
    //private:
    //	int			 m_channels;
    //	unsigned int m_offsetR;
    //	unsigned int m_offsetG;
    //	unsigned int m_offsetB;
    //	unsigned int m_offsetA;
    //
    //	ColorImage& operator = (const ColorImage& rhs);
    //	ColorImage(const ColorImage& other);
    //
    //	bool readCV(const std::string& fileName);
    //
    //	void writeCV(const std::string& fileName) const;
    //	void writeCVHelper(const std::string& fileName, T tmp[]) const;
    //
    //	void allocateMemory() { allocateMemory(m_width, m_height, m_channels); }
    //	void allocateMemory(int width, int height, int numChan);
    //	void setRGBAOffsets();	// TODO: Fix need for this somehow with specialization
};



template<ColorSpace C>
class ColorImage<unsigned char, C> : public Image < unsigned char >
{
public:
    // TODO: Friending all specializations to gain access to their 
    // internals. No idea how to circumvent this yet.
    friend class ColorImage < unsigned char, ColorSpace::CS_GRAY >;
    friend class ColorImage < unsigned char, ColorSpace::CS_RGB >;
    friend class ColorImage < unsigned char, ColorSpace::CS_BGR >;
    friend class ColorImage < unsigned char, ColorSpace::CS_HSV >;
    friend class ColorImage < unsigned char, ColorSpace::CS_LAB >;
    friend class ColorImage < unsigned char, ColorSpace::CS_RGBA >;
    friend class ColorImage < unsigned char, ColorSpace::CS_BGRA >;
    friend class ColorImage < unsigned char, ColorSpace::CS_ARGB >;
    friend class ColorImage < float, ColorSpace::CS_GRAY >;
    friend class ColorImage < float, ColorSpace::CS_RGB >;
    friend class ColorImage < float, ColorSpace::CS_BGR >;
    friend class ColorImage < float, ColorSpace::CS_HSV >;
    friend class ColorImage < float, ColorSpace::CS_LAB >;
    friend class ColorImage < float, ColorSpace::CS_RGBA >;
    friend class ColorImage < float, ColorSpace::CS_BGRA >;
    friend class ColorImage < float, ColorSpace::CS_ARGB >;

    static int getNumberOfChannels(const ColorSpace colorSpace);
    static int getCvType(const ColorSpace colorSpace);

    ColorImage();
    ColorImage(int width, int height);
    explicit ColorImage(const std::string& fileName);

    ColorImage(const ColorImage& other);    // Copy Constructor
    ColorImage(ColorImage&& other);         // C++11 Move Contructor

    ~ColorImage();

    ColorImage& operator =(const ColorImage& rhs);  // Copy Assignment Operator
    // UNDONE:
    //ColorImage& operator =(ColorImage&& rhs); // C++11 Move Assignment Operator

    bool read(const std::string& fileName);
    void write(const std::string& fileName) const;
    void copy(ColorImage* output) const;

    // INFO: Direct conversion between HSV and Grey does not work as intended
    // (converion assumes HSV values as RGB values and vice versa). Use another
    // conversion to RGB to circumvent this.
    template<ColorSpace D>
    void convertColorSpace(ColorImage<unsigned char, D>* output) const;
    void convertType(ColorImage<float, C>* output) const;

    template<typename U, ColorSpace D>
    bool compareDimensions(const ColorImage<U, D>& other) const;

    // Warning: Current image becomes invalid!
    void resize(int width, int height);

    void setToBlack();
    void setToValue(unsigned char value);
    void setToValue(unsigned char r,
                    unsigned char g,
                    unsigned char b,
                    unsigned char a = 255);

    // Accessors

    int R(int pixelNo) const;
    int G(int pixelNo) const;
    int B(int pixelNo) const;
    int A(int pixelNo) const;
    int H(int pixelNo) const;
    int S(int pixelNo) const;
    int V(int pixelNo) const;

    int R(int x, int y) const;
    int G(int x, int y) const;
    int B(int x, int y) const;
    int A(int x, int y) const;
    int H(int x, int y) const;
    int S(int x, int y) const;
    int V(int x, int y) const;

    // Getters
    int             size() const;
    int	            getChannels()  const;
    unsigned int	getOffsetR()   const;
    unsigned int	getOffsetG()   const;
    unsigned int	getOffsetB()   const;
    unsigned int	getOffsetA()   const;

private:
    int          m_channels;
    unsigned int m_offsetR;
    unsigned int m_offsetG;
    unsigned int m_offsetB;
    unsigned int m_offsetA;

    void convertToHsv();
    void convertFromHsv();
    void convertColorToHsv(float  r,
                           float  g,
                           float  b,
                           float* h,
                           float* s,
                           float* v);

    void convertColorFromHsv(float  h,
                             float  s,
                             float  v,
                             float* r,
                             float* g,
                             float* b);

    bool readCv(const std::string& fileName);

    void setRGBAOffsets();
};

template<ColorSpace C>
class ColorImage<float, C> : public Image < float >
{
public:
    // TODO: Friending all specializations to gain access to their 
    // internals. No idea how to circumvent this yet.
    friend class ColorImage < unsigned char, ColorSpace::CS_GRAY >;
    friend class ColorImage < unsigned char, ColorSpace::CS_RGB >;
    friend class ColorImage < unsigned char, ColorSpace::CS_BGR >;
    friend class ColorImage < unsigned char, ColorSpace::CS_HSV >;
    friend class ColorImage < unsigned char, ColorSpace::CS_LAB >;
    friend class ColorImage < unsigned char, ColorSpace::CS_RGBA >;
    friend class ColorImage < unsigned char, ColorSpace::CS_BGRA >;
    friend class ColorImage < unsigned char, ColorSpace::CS_ARGB >;
    friend class ColorImage < float, ColorSpace::CS_GRAY >;
    friend class ColorImage < float, ColorSpace::CS_RGB >;
    friend class ColorImage < float, ColorSpace::CS_BGR >;
    friend class ColorImage < float, ColorSpace::CS_HSV >;
    friend class ColorImage < float, ColorSpace::CS_LAB >;
    friend class ColorImage < float, ColorSpace::CS_RGBA >;
    friend class ColorImage < float, ColorSpace::CS_BGRA >;
    friend class ColorImage < float, ColorSpace::CS_ARGB >;

    static int getNumberOfChannels(const ColorSpace colorSpace);
    static int getCvType(const ColorSpace colorSpace);

    ColorImage();
    ColorImage(int width, int height);
    explicit ColorImage(const std::string& fileName);

    ColorImage(const ColorImage& other);    // Copy Constructor
    ColorImage(ColorImage&& other);         // C++11 Move Contructor

    ~ColorImage();

    ColorImage& operator =(const ColorImage& rhs);  // Copy Assignment Operator
    // UNDONE:
    //ColorImage& operator =(ColorImage&& rhs); // C++11 Move Assignment Operator

    bool read(const std::string& fileName);
    void write(const std::string& fileName) const;
    void copy(ColorImage* output) const;

    // INFO: Direct conversion between HSV and Grey does not work as intended
    // (converion assumes HSV values as RGB values and vice versa). Use another
    // conversion to RGB to circumvent this.
    template<ColorSpace D>
    void convertColorSpace(ColorImage<float, D>* output) const;
    void convertType(ColorImage<unsigned char, C>* output) const;

    template<typename U, ColorSpace D>
    bool compareDimensions(const ColorImage<U, D>& other) const;

    // Warning: Current image becomes invalid!
    void resize(int width, int height);

    void setToBlack();
    void setToValue(float f);
    void setToValue(float r, float g, float b, float a = 1.0f);

    // Accessors

    int R(int pixelNo) const;
    int G(int pixelNo) const;
    int B(int pixelNo) const;
    int A(int pixelNo) const;
    int H(int pixelNo) const;
    int S(int pixelNo) const;
    int V(int pixelNo) const;

    int R(int x, int y) const;
    int G(int x, int y) const;
    int B(int x, int y) const;
    int A(int x, int y) const;
    int H(int x, int y) const;
    int S(int x, int y) const;
    int V(int x, int y) const;

    // Getters
    int	         getChannels() const;
    unsigned int getOffsetR()  const;
    unsigned int getOffsetG()  const;
    unsigned int getOffsetB()  const;
    unsigned int getOffsetA()  const;

private:
    int          m_channels;
    unsigned int m_offsetR;
    unsigned int m_offsetG;
    unsigned int m_offsetB;
    unsigned int m_offsetA;

    void convertToHsv();
    void convertFromHsv();
    void convertColorToHsv(float  r,
                           float  g,
                           float  b,
                           float* h,
                           float* s, 
                           float* v);

    void convertColorFromHsv(float  h,
                             float  s,
                             float  v,
                             float* r,
                             float* g,
                             float* b);

    void convertToLab();
    void convertFromLab();
    void convertColorToLab(float  r,
                           float  g,
                           float  b,
                           float* lo,
                           float* ao,
                           float* bo);

    void convertColorFromLab(float  lIn,
                             float  gIn,
                             float  bIn,
                             float* r,
                             float* g,
                             float* b);

    bool readCv(const std::string& fileName);

    void setRGBAOffsets();
};

//	To define methods for this template include source file.
//	Must be excluded from project/ignored.
#include "ColorImage.cpp"