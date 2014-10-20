#define _CRT_SECURE_NO_WARNINGS		//TODO To be able to use the fopen() function

#pragma once

#include "Image.h"

#include <string>

/** 
*	Defines various color spaces.
*	Declared outside of class because of template.
**/
enum class ColorSpace { CS_GRAY = 0, CS_RGB, CS_HSV, CS_LAB, CS_RGBA, CS_BGRA, CS_ARGB, CS_BGR};

template <typename Type, ColorSpace cs>
class ColorImage : public Image<Type>
{
public:
	ColorImage( void ) { };
	~ColorImage( void ) { };
	
	bool read(const std::string &fileName);
	
	int getNumChannels() const;

private:

	bool isRawFilename(const std::string &fileName) const;
	bool isBinaryFilename(const std::string &fileName) const;
	
	bool readRaw(const std::string &fileName);
	bool readBinary(const std::string &fileName);
	bool readCV(const std::string &fileName);
	
	void setRGBAOffsets();
};

//	To define methods for this template include source file.
//	Must be excluded from project/ignored.
#include "ColorImage.cpp"