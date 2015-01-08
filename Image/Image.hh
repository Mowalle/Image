/**
*	This file must be excluded from build!
**/

const float Image<float>::ValueMin = 0;
const float Image<float>::ValueMax = 1.0f;
const unsigned char Image<unsigned char>::ValueMin = 0;
const unsigned char Image<unsigned char>::ValueMax = 255;

template <typename T>
Image<T>::Image() : m_data( nullptr ),
                    m_width( 0 ), 
					m_height( 0 ),
					m_numChan( 0 ),
					m_memAllocated( 0 )
{
}

template <typename T>
Image<T>::Image( int width, int height ) : m_data( nullptr ),
                                           m_width( width ),
										   m_height( height ),
										   m_numChan( 0 ),
										   m_memAllocated( 0 )
{ 
}

template <typename T>
Image<T>::~Image()
{ 
	delete[] m_data;
}


template <typename T>
T* Image<T>::getData() const
{
	return m_data;
}


template <typename T>
int Image<T>::getWidth() const
{
	return m_width;
}


template <typename T>
int Image<T>::getHeight() const
{
	return m_height;
}

template <typename T>
int Image<T>::getMemAllocated() const
{
	return m_memAllocated;
}

template <typename T>
void Image<T>::setWidth( int width )
{
	m_width = width;
}

template <typename T>
void Image<T>::setHeight( int height )
{
	m_height = height;
}

/** ------------------------------------
*				  protected
*	------------------------------------
**/

template<typename T>
T Image<T>::convertValue( T value, T )
{
	return ( value * 1 );
}

template<>
template<>
unsigned char Image<float>::convertValue( float value, unsigned char )
{
	return ( value * 255 );
}

template<>
template<>
float Image<unsigned char>::convertValue( unsigned char value, float )
{
	return ( value * ( 1 / 255.0f ) );
}