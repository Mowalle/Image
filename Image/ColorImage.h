#define _CRT_SECURE_NO_WARNINGS		//TODO To be able to use the fopen() function

#pragma once

#include "Image.h"

#include<iostream>	// std::cout, std::clog, std::cerr, std::endl
#include<string>
#include<fstream>	// dummy file reading
#include<algorithm>	// std::max(), std::min(), std::copy()

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>

/**
*	Defines various color spaces.
*	Declared outside of class because needed as template parameter.
**/
enum class ColorSpace
{
    CS_GRAY = 0, CS_RGB, CS_BGR, CS_HSV, CS_LAB, CS_RGBA, CS_BGRA, CS_ARGB
};


template < typename T, ColorSpace C >
class ColorImage : public Image < T >
{
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
    
    ~ColorImage();

    bool read(const std::string& fileName);
    void write(const std::string& fileName) const;
    void copyTo(ColorImage* output) const;
    // INFO: Direct conversion between HSV and Grey does not work as intended
    // (converion assumes HSV values as RGB values and vice versa). Use another
    // conversion to RGB to circumvent this.
    template<ColorSpace D>
        void convertColorSpace(ColorImage<unsigned char, D>* output) const;
    void convertType(ColorImage<float, C>* output) const;
    template<typename U, ColorSpace D>
        bool sameSize(const ColorImage<U, D>& other) const;
    // Warning: Current image becomes invalid!
    void resize(int width, int height);
	void setPixelColor(int pixelIdx, 
					   unsigned char r,
					   unsigned char g,
					   unsigned char b,
					   unsigned char a = 255);
    void setToBlack();
    void setToValue(unsigned char value);
    void setToValue(unsigned char r,
                    unsigned char g,
                    unsigned char b,
                    unsigned char a = 255);

    // Accessors

    int R(int pixelIdx) const;
    int G(int pixelIdx) const;
    int B(int pixelIdx) const;
    int A(int pixelIdx) const;
    int H(int pixelIdx) const;
    int S(int pixelIdx) const;
    int V(int pixelIdx) const;

    int R(int x, int y) const;
    int G(int x, int y) const;
    int B(int x, int y) const;
    int A(int x, int y) const;
    int H(int x, int y) const;
    int S(int x, int y) const;
    int V(int x, int y) const;

    // Getters
    int	         getChannels()  const;
    unsigned int getOffsetR()   const;
    unsigned int getOffsetG()   const;
    unsigned int getOffsetB()   const;
    unsigned int getOffsetA()   const;

private:
    int          m_channels;
    unsigned int m_offsetR;
    unsigned int m_offsetG;
    unsigned int m_offsetB;
    unsigned int m_offsetA;

    void convertToHsv();
    void convertFromHsv();
    void convertColorToHsv(float  r, float  g, float  b,
                           float* h, float* s, float* v);
    void convertColorFromHsv(float  h, float  s, float  v,
                             float* r, float* g, float* b);
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

    ~ColorImage();

    bool read(const std::string& fileName);
    void write(const std::string& fileName) const;
    void copyTo(ColorImage* output) const;

    // INFO: Direct conversion between HSV and Grey does not work as intended
    // (converion assumes HSV values as RGB values and vice versa). Use another
    // conversion to RGB to circumvent this.
    template<ColorSpace D>
        void convertColorSpace(ColorImage<float, D>* output) const;
    void convertType(ColorImage<unsigned char, C>* output) const;

    template<typename U, ColorSpace D> 
        bool sameSize(const ColorImage<U, D>& other) const;
	// Warning: Current image becomes invalid!
    // UNDONE(Ralf): Resize while retaining old image.
    void resize(int width, int height);
	void setPixelColor(int pixelIdx, 
					   float r,
					   float g,
					   float b,
					   float a = 1.0f);
    void setToBlack();
    void setToValue(float f);
    void setToValue(float r, float g, float b, float a = 1.0f);

    // Accessors

    int R(int pixelIdx) const;
    int G(int pixelIdx) const;
    int B(int pixelIdx) const;
    int A(int pixelIdx) const;
    int H(int pixelIdx) const;
    int S(int pixelIdx) const;
    int V(int pixelIdx) const;

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
    void convertColorToHsv(float  r, float  g, float  b,
                           float* h, float* s, float* v);
    void convertColorFromHsv(float  h, float  s, float  v,
                             float* r, float* g, float* b);
    void convertToLab();
    void convertFromLab();
    void convertColorToLab(float  r,  float  g,  float  b,
                           float* lo, float* ao, float* bo);

    void convertColorFromLab(float  lIn, float  gIn, float  bIn,
                             float* r,   float* g,   float* b);
    bool readCv(const std::string& fileName);
    void setRGBAOffsets();
};

//	To define methods for this template include source file.
//	Must be excluded from project/ignored.
#include "ColorImage.cpp"