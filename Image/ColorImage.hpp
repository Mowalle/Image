#pragma once

#include "Image.hpp"

#include<iostream> // std::cout, std::clog, std::cerr, std::endl
#include<fstream> // file reading
#include<algorithm> // std::max(), std::min(), std::copy()
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>


/**
*	Defines various color spaces.
*	Declared outside of class because needed as template parameter.
**/

enum class ColorSpace
{
    GRAY,
    RGB,
    BGR,
    HSV,
    LAB,
    RGBA,
    BGRA,
    ARGB
};


template <class T, ColorSpace C>
class ColorImage : public Image<T>
{
    // FIX: This is pretty much hardcoded, but avoids partial specialization of entire class
    // and thus a lot of code duplication. If there are other ways to do this, fixes welcome.
    static_assert(std::is_same<T, float>::value || 
                  std::is_same<T, unsigned char>::value,
                  "ColorImage only accepts float and unsigned char as first template parameter!");

    
    friend class ColorImage<unsigned char, ColorSpace::GRAY>;
    friend class ColorImage<unsigned char, ColorSpace::RGB>;
    friend class ColorImage<unsigned char, ColorSpace::BGR>;
    friend class ColorImage<unsigned char, ColorSpace::HSV>;
    friend class ColorImage<unsigned char, ColorSpace::LAB>;
    friend class ColorImage<unsigned char, ColorSpace::RGBA>;
    friend class ColorImage<unsigned char, ColorSpace::BGRA>;
    friend class ColorImage<unsigned char, ColorSpace::ARGB>;
    friend class ColorImage<float, ColorSpace::GRAY>;
    friend class ColorImage<float, ColorSpace::RGB>;
    friend class ColorImage<float, ColorSpace::BGR>;
    friend class ColorImage<float, ColorSpace::HSV>;
    friend class ColorImage<float, ColorSpace::LAB>;
    friend class ColorImage<float, ColorSpace::RGBA>;
    friend class ColorImage<float, ColorSpace::BGRA>;
    friend class ColorImage<float, ColorSpace::ARGB>;

public:
    ColorImage();
    ColorImage(int width, int height);
    explicit ColorImage(const std::string& fileName);

    virtual ~ColorImage();
    
    static int getNumChannels();
    static int getCvType();

    virtual bool read(const std::string& fileName) override;
    virtual void write(const std::string& fileName) const override;
    void copyTo(ColorImage* output) const;
    // INFO: Direct conversion between HSV and Grey does not work as intended
    // (converion assumes HSV values as RGB values and vice versa). Use another
    // conversion to RGB to circumvent this.
    template <ColorSpace D>
    void convertColorSpace(ColorImage<T, D>* output) const;
    template <class U>
    void convertType(ColorImage<U, C>* output) const;
    template <typename U, ColorSpace D>
    bool sameSize(const ColorImage<U, D>& other) const;
    // Warning: Current image becomes invalid!
    virtual void resize(int width, int height) override;
    void setPixelColor(int pixelIdx, T r, T g, T b, T a);
    void setToBlack();
    void setToValue(T value);
    void setToColor(T r, T g, T b);
    void setToColor(T r, T g, T b, T a);

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
    int getChannels() const;
    unsigned int getOffsetR() const;
    unsigned int getOffsetG() const;
    unsigned int getOffsetB() const;
    unsigned int getOffsetA() const;

private:
    int m_channels;
    unsigned int m_offsetR;
    unsigned int m_offsetG;
    unsigned int m_offsetB;
    unsigned int m_offsetA;

    void convertToHsv();
    void convertFromHsv();
    static void convertColorToHsv(float r, float g, float b,
                                  float* h, float* s, float* v);
    static void convertColorFromHsv(float h, float s, float v,
                                    float* r, float* g, float* b);
    bool readCv(const std::string& fileName);
    void setRGBAOffsets();

    template<class From, class To>
    static To getColorValueFactor(From, To)
    {
        return static_cast<To>(1.0);
    }

    template<>
    static float getColorValueFactor(unsigned char, float)
    {
        return 1 / 255.0f;
    }

    template<>
    static unsigned char getColorValueFactor(float, unsigned char)
    {
        return 255;
    }

};

//	To define methods for this template include source file.
//	Must be excluded from project/ignored.
#include "ColorImage.cpp"