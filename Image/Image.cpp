/**
*	This file must be excluded from build!
**/

template<typename T>
Image<T>::Image() : 
    m_width(0), 
    m_height(0),
    m_allocatedMemory(0),
    m_data(nullptr)
{
}

template<typename T>
Image<T>::Image(int width, int height) : 
    m_width(width),
    m_height(height),
    m_allocatedMemory(0),
    m_data(nullptr)
{

}

template<typename T>
Image<T>::Image(const Image& other) : 
    m_width(other.m_width), 
    m_height(other.m_height), 
    m_allocatedMemory(other.m_allocatedMemory),
    m_data(m_allocatedMemory ? new unsigned char[m_allocatedMemory] : nullptr)
{
    std::copy(other.m_data, other.m_data + m_allocatedMemory, m_data);
}

// UNDONE: Move constructor
//template<typename T>
//Image<T>::Image(Image&& other) :
//    m_data(other.m_data),
//    m_width(other.m_width),
//    m_height(other.m_height),
//    m_allocatedMemory(other.m_allocatedMemory)
//{
//    // reset other
//    other.m_width = 0;
//    other.m_height = 0;
//    other.m_allocatedMemory = 0;
//    other.m_data = nullptr;
//}


template<typename T>
Image<T>::~Image()
{ 
    delete [] m_data;
}


template<typename T>
Image<T>& Image<T>::operator =(const Image& rhs)
{
    if (this != &rhs)
    {
        // remove existing resources
        delete [] m_data;
        m_data = nullptr;

        // copy over resources
        m_width    = rhs.m_width;
        m_height   = rhs.m_height;

        m_allocatedMemory = rhs.m_allocatedMemory;
        m_data = new unsigned char[m_allocatedMemory];
        for (int i = 0; i<m_allocatedMemory; ++i)
        {
            m_data[i] = rhs.m_data[i];
        }
    }
    return *this;
}


// UNDONE: Move Assignment Operator
//template<typename T>
//Image<T>& Image<T>::operator =(Image&& rhs)
//{
//    if (this != &rhs)
//    {
//        // release current object's resources
//        delete[] m_data;
//        m_data = nullptr;
//        m_width = 0;
//        m_height = 0;
//        m_allocatedMemory = 0;
//
//        // pilfer rhs's resources
//        m_data = rhs.m_data;
//        m_width = rhs.m_width;
//        m_height = rhs.m_height;
//        m_allocatedMemory = rhs.m_allocatedMemory;
//
//        // reset rhs
//        rhs.m_width = 0;
//        rhs.m_height = 0;
//        rhs.m_allocatedMemory = 0;
//        rhs.m_data = nullptr;
//    }
//    return *this;
//}

template<typename T>
int Image<T>::size() const
{
    return m_width * m_height;
}


template<typename T>
T* Image<T>::getData() const
{
    return m_data;
}


template<typename T>
int Image<T>::getWidth() const
{
    return m_width;
}


template<typename T>
int Image<T>::getHeight() const
{
    return m_height;
}

template<typename T>
int Image<T>::getAllocatedMemory() const
{
    return m_allocatedMemory;
}


/** ------------------------------------
*				  protected
*	------------------------------------
**/