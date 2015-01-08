#define _CRT_SECURE_NO_WARNINGS		//TODO To be able to use the fopen() function

#pragma once

#include "Image.h"

#include <iostream>
#include <string>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

/**
*	Defines various color spaces.
*	Declared outside of class because of template.
**/
enum class ColorSpace { CS_GRAY = 0, CS_RGB, CS_HSV, CS_LAB, CS_RGBA, CS_BGRA, CS_ARGB, CS_BGR };

template <typename T, ColorSpace C>
class ColorImage : public Image < T >
{
public:
	// Implementing each constructor because C++11 (which features constructor delegation) is only partially implemented in VS2012.

	ColorImage();
	ColorImage( int width, int height );
	explicit ColorImage( const std::string& fileName );

	~ColorImage();

	static int getNumberOfChannels( const ColorSpace colorsp );

	bool read( const std::string &fileName );
	void write( const std::string &fileName ) const;
	void copy( ColorImage* src ) const;

	template< typename U, ColorSpace OtherClrSpace >
	void convert( ColorImage<U, OtherClrSpace>* out ) const;

	// Accessors

	// Returns the (n + 1) red-value-index from the image's data
	int R( int n ) const { return n * m_numChan + m_offsetR; }
	// Returns the (n + 1) green-value-index from the image's data
	int G( int n ) const { return n * m_numChan + m_offsetG; }
	// Returns the (n + 1) blue-value-index from the image's data
	int B( int n ) const { return n * m_numChan + m_offsetB; }
	// Returns the (n + 1) alpha-value-index from the image's data
	int A( int n ) const { return n * m_numChan + m_offsetA; }
	// Returns the (n + 1) hue-value-index (red-value-index) from the image's data
	int H( int n ) const { return n * m_numChan + m_offsetR; }
	// Returns the (n + 1) saturation-value-index (green-value-index) from the image's data
	int S( int n ) const { return n * m_numChan + m_offsetG; }
	// Returns the (n + 1) value-value-index (blue-value-index) from the image's data
	int V( int n ) const { return n * m_numChan + m_offsetB; }

	// Getters
	int getNumberOfChannels() const;
	unsigned int getOffsetR() const;
	unsigned int getOffsetG() const;
	unsigned int getOffsetB() const;
	unsigned int getOffsetA() const;
	
private:

	int			 m_numChan;
	unsigned int m_offsetR;
	unsigned int m_offsetG;
	unsigned int m_offsetB;
	unsigned int m_offsetA;

	ColorImage& operator = ( const ColorImage& rhs );
	ColorImage( const ColorImage& other );

	bool readCV( const std::string& fileName );

	void writeCV( const std::string& fileName ) const;
	void writeCVHelper( const std::string& fileName, T tmp[] ) const;

	void reallocateMemory() { reallocateMemory( m_width, m_height, m_numChan); }
	void reallocateMemory( int width, int height, int numChan );
	void setRGBAOffsets();	// TODO: Fix need for this somehow with specialization
};

//	To define methods for this template include source file.
//	Must be excluded from project/ignored.
#include "ColorImage.hh"