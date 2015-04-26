#pragma once

#include <vector>

template <class T>
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
    virtual void resize(int width, int height);
    int size() const;

    // Getter
    std::vector<T>& getData();  // For manipulating the data afterwards.
    const std::vector<T>& getData() const;
    int getWidth()  const;
    int getHeight() const;

    void setWidth(int width);
    void setHeight(int height);

protected :
    int m_width;
    int m_height;
    std::vector<T> m_data;
};

#include "Image.cpp"