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
enum class ColorSpace { CS_GRAY = 0, CS_RGB, CS_HSV, CS_LAB, CS_RGBA, CS_BGRA, CS_ARGB, CS_BGR};

template <typename Type, ColorSpace cs>
class ColorImage : public Image<Type>
{
public:
	ColorImage() : m_numChan( getNumberOfChannels( cs ) ) {};
	~ColorImage(){};
	
	// TODO: Fix need for this with specialisation AND move this to the .cpp
	static int getNumberOfChannels ( ColorSpace colorsp )
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

	bool read( const std::string &fileName );

	// Accessors
	// Returns the (n + 1) red-value-index from the image's data
	int r( int n ) const { return n * m_numChan + m_offsetR; }
	// Returns the (n + 1) green-value-index from the image's data
	int g( int n ) const { return n * m_numChan + m_offsetG; }
	// Returns the (n + 1) blue-value-index from the image's data
	int b( int n ) const { return n * m_numChan + m_offsetB; }
	// Returns the (n + 1) alpha-value-index from the image's data
	int a( int n ) const { return n * m_numChan + m_offsetA; }
	// Returns the (n + 1) hue-value-index (red-value-index) from the image's data
	int h( int n ) const { return n * m_numChan + m_offsetR; }
	// Returns the (n + 1) saturation-value-index (green-value-index) from the image's data
	int s( int n ) const { return n * m_numChan + m_offsetG; }
	// Returns the (n + 1) value-value-index (blue-value-index) from the image's data
	int v( int n ) const { return n * m_numChan + m_offsetB; }

	// Getters

	int getNumberOfChannels() const { return m_numChan; }

private:

	bool isFileType( const std::string &fileName, const std::string &fileType ) const;
	
	bool readBinary( const std::string &fileName );
	bool readCV( const std::string &fileName );
	bool readRaw( const std::string &fileName );

	void copyDataFromCV( const cv::Mat &cvImage );
	void reallocateMemory();
	void setRGBAOffsets();	// TODO: Fix need for this somehow with specialisation

	int			 m_numChan;
	unsigned int m_offsetR;
	unsigned int m_offsetG;
	unsigned int m_offsetB;
	unsigned int m_offsetA;

};

//template <typename Type>
//class ColorImage<Type, ColorSpace::CS_RGBA> : public Image<Type>
//{
//public:
//	bool read( const std::string &fileName ) {return false;};
//};

//	To define methods for this template include source file.
//	Must be excluded from project/ignored.
#include "ColorImage.cpp"