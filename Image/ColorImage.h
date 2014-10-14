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
	//bool readBinary(int width, int height, unsigned int *bitImg);
	bool readCV(const std::string &fileName);
	
	void				setRGBAOffsets();
};

/** ------------------------------------
*	------------------------------------
*
*	Inline definitions because template.
*
*	------------------------------------
*	------------------------------------
**/


/** ------------------------------------
*					public
*	------------------------------------
**/
template <typename Type, ColorSpace cs>
int ColorImage<Type, cs>::getNumChannels() const
{
	switch(cs)
	{
	case ColorSpace::CS_RGB || ColorSpace CS::HSV:
		return 3;
		break;
	}
}


/** ------------------------------------
*					private
*	------------------------------------
**/

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::read(const std::string &fileName)
{
	// Create dummy file and check if it was a success before trying to 
	// read image.
	FILE *dummy = fopen(fileName.c_str(), "r");

	if (dummy == nullptr)
	{
		// using fileName.c_str(), else the file name would be displayed incorretly.
		fprintf(stderr, "ERROR: Couldn't open file %s\n", fileName.c_str());
		return false;
	}

	fclose(dummy);

	bool success;
	// Look at file name. Differentiate between formats.
	if (isRawFilename(fileName))
	{
		success = readRaw(fileName);
	}
	else if (isBinaryFilename(fileName))
	{
		success = readBinary(fileName);
	}
	else
	{
		// Use OpenCV to read image, replacing readCV(fileName, cf)
		success = readCV(fileName);
	}

	return success;
}

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::isRawFilename( const std::string &fileName ) const
{
	return false;
}

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::isBinaryFilename( const std::string &fileName ) const
{
	return false;
}

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::readRaw( const std::string &fileName )
{
	return false;
}

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::readBinary( const std::string &fileName )
{
	return false;
}

template <typename Type, ColorSpace cs>
bool ColorImage<Type, cs>::readCV( const std::string &fileName )
{
	return false;
}