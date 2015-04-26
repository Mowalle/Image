/**
*	This file must be excluded from build!
**/

template <class T>
Image<T>::Image() : m_width(0), m_height(0)
{}


template <class T>
Image<T>::Image(unsigned int width, unsigned int height) 
: m_width(width), m_height(height)
{
    m_data.resize(width * height);
    std::clog << "Image width/height constructor." << std::endl;
}


template <class T>
Image<T>::~Image()
{}


template <class T>
int Image<T>::size() const
{
    return m_width * m_height;
}


template <class T>
void Image<T>::resize(int width, int height)
{
    m_width = width;
    m_height = height;
    m_data.resize(m_width * m_height);
}


template <class T>
std::vector<T>& Image<T>::getData()
{
    return m_data;
}


template <class T>
const std::vector<T>& Image<T>::getData() const
{
    return m_data;
}


template <class T>
int Image<T>::getWidth() const
{
    return m_width;
}


template <class T>
void Image<T>::setWidth(int width)
{
    resize(width, m_height);
}


template <class T>
int Image<T>::getHeight() const
{
    return m_height;
}


template <class T>
void Image<T>::setHeight(int height)
{
    resize(m_width, height);
}