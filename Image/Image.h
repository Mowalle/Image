#pragma once

#include <iostream>
#include <string>

template <typename T>
class Image
{
public:

	// These two constants contain the min and max for the currently used value scale.
	// For example, float has ValueMin = 0 and ValueMax = 1.0f.
	//      unsigned char has ValueMin = 0 and ValueMax = 255.
	// This is to prevent the need for complicated (partial) specialization in the derivative
	// classes, as well as the possibility to "ask for the type" of the template class.
	// When implementing new Types for the Image-class, bear the definitions of these
	// values in the Image.hh file (top) in mind.
	const static T ValueMin, ValueMax;

	// Constructor(s)
	Image();
	Image( int width, int height );

	// Destructor(s)
	virtual ~Image();

	virtual bool read( const std::string& fileName ) = 0;
	virtual void write( const std::string& fileName ) const = 0;
	
	// Getter
	T*  getData()         const;
	int getWidth()        const;
	int getHeight()       const;
	int getMemAllocated() const;

	// Setter
	void setWidth( int width );
	void setHeight( int height );
	
	// UNDONE: The following are made public to prevent Compiler Error C2248: cannot access 'access' member declared in class 'class'
	// when used with Image<unsigned char>::convertValue(...) or other specialized types.
	// More information here: http://msdn.microsoft.com/en-us/library/tsbce2bh.aspx
	// Should be made protected somehow.
	template<typename U>
	static U convertValue( T f, U );
	static T convertValue( T f, T );

		
protected:
	
	T*  m_data;
	int	m_width;
	int	m_height;
	int m_numChan;
	int	m_memAllocated;

	virtual void reallocateMemory() = 0;
	
private:
	
	Image& operator = ( const Image& rhs );
	Image ( const Image& other );

};

#include "Image.hh"