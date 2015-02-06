#pragma once

#include<iostream>
#include<string>

template<typename T>
class Image
{
public :
    // Constructor(s)
    Image();
    Image(int width, int height);

    // Copy Constructor
    Image(const Image& other);

    // UNDONE: C++11 Move Constructor
    //Image(Image&& other);

    // Destructor
    virtual ~Image();
    
    Image& operator =(const Image& rhs);  // Copy Assignment Operator
    // UNDONE: C++11 Move Assignment Operator
    //Image& operator =(Image&& rhs); 

    virtual bool read(const std::string& fileName) = 0;
    virtual void write(const std::string& fileName) const = 0;
        
    // Getter
    int size()       const;
    T*  getData()            const;
    int getWidth()           const;
    int getHeight()          const;
    int getAllocatedMemory() const;

            
protected :
    
    int	m_width;
    int	m_height;
    int	m_allocatedMemory;
    T*  m_data;
};

#include "Image.cpp"