#pragma once

#include <iostream>
#include <string>

template <typename FormatT>
class Image
{
public:
	// Constructor(s)
	Image();
	Image( int width, int height );

	// Destructor(s)
	virtual ~Image();

	virtual bool read( const std::string& fileName ) = 0;
	virtual void write( const std::string& fileName ) const = 0;

	// Getter
	FormatT* getData() const { return m_data; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	int getMemAllocated() const { return m_memAllocated; }

	// Setter
	void setWidth(int width) { m_width = width; }
	void setHeight(int height) { m_height = height; }
	
protected:
	FormatT*			m_data;
	int					m_width;
	int					m_height;
	int                 m_numChan;
	int					m_memAllocated;

	virtual void reallocateMemory() = 0;

private:
	Image& operator = ( const Image& rhs );
	Image ( const Image& other );

	
};

#include "Image.hh"