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

template <typename FormatT, ColorSpace cs>
class ColorImage : public Image < FormatT >
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

	template< typename OtherFormatT, ColorSpace OtherClrSpace >
	void convert( ColorImage<OtherFormatT, OtherClrSpace>* out ) const;

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

	int getNumberOfChannels() const { return m_numChan; }
	unsigned int getOffsetR() const { return m_offsetR; }
	unsigned int getOffsetG() const { return m_offsetG; }
	unsigned int getOffsetB() const { return m_offsetB; }
	unsigned int getOffsetA() const { return m_offsetA; }
	
private:

	ColorImage& operator = ( const ColorImage& rhs );
	ColorImage( const ColorImage& other );

	bool readCV( const std::string &fileName );

	// This function is fully specialized for each color space.
	// Actual implementation inside copyDataFromCVHelper(...).
	void writeCV( const std::string &fileName ) const;
	// HELPER FUNCTION: Specialized writeCV calls this, with scale depending on specialization
	void writeCVHelper( FormatT* trgData, float scale ) const;

	void reallocateMemory() { reallocateMemory( m_width, m_height, m_numChan); }
	void reallocateMemory( int width, int height, int numChan );
	void setRGBAOffsets();	// TODO: Fix need for this somehow with specialization

	int			 m_numChan;
	unsigned int m_offsetR;
	unsigned int m_offsetG;
	unsigned int m_offsetB;
	unsigned int m_offsetA;

};

//	To define methods for this template include source file.
//	Must be excluded from project/ignored.
#include "ColorImage.hh"