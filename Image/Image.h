#pragma once

#include <string>

template <typename Type>
class Image
{
public:
	Image( ) 
	: m_data(0)
	, m_width(0)
	, m_height(0)
	, m_memAllocated(0)
	{ };
	~Image( ) { };

	//virtual bool read(const std::string &filename) = 0;

	// Getter
	Type getData() { return m_data; }
	int getWidth() { return m_width; }
	int getHeight() { return m_height; }
	int getMemAllocated() { return m_memAllocated; }

	// Setter
	void setWidth(int width) { m_width = width; }
	void setHeight(int height) { m_height = height; }

protected:
	Type				m_data;
	int					m_width;
	int					m_height;
	int					m_memAllocated;
};

