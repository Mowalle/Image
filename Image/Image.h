#pragma once

#include <vector>

template<typename T>
class Image 
{
public :
    // Constructor(s)
    Image();
    Image(unsigned int width, unsigned int height);

    // Destructor
    virtual ~Image();
    
    virtual bool read(const std::string& fileName) = 0;
    virtual void write(const std::string& fileName) const = 0;
        
    unsigned int size() const;

    // Getter
    std::vector<T>& getData();  // For manipulating the data afterwards.
    const std::vector<T>& getData() const;
    unsigned int getWidth()  const;
    unsigned int getHeight() const;

protected :
    unsigned int m_width;
    unsigned int m_height;
    std::vector<T> m_data;
};

#include "Image.cpp"