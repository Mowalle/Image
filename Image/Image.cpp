/**
*	This file must be excluded from build!
**/

template<typename T>
Image<T>::Image() : m_width(0), m_height(0)
{}


template<typename T>
Image<T>::Image(unsigned int width, unsigned int height) : m_width(width), m_height(height)
{
    m_data.resize(width * height);
}


template<typename T>
Image<T>::~Image()
{}


template<typename T>
unsigned int Image<T>::size() const
{
    return m_width * m_height;
}


template<typename T>
const std::vector<T>& Image<T>::getData() const
{
    return m_data;
}


template<typename T>
unsigned int Image<T>::getWidth() const
{
    return m_width;
}


template<typename T>
unsigned int Image<T>::getHeight() const
{
    return m_height;
}