/**
*	This file must be excluded from build!
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