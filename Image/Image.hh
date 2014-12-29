/**
*	This file must be excluded from build!
**/

const float Image<float>::ValueMin = 0;
const float Image<float>::ValueMax = 1.0f;
const unsigned char Image<unsigned char>::ValueMin = 0;
const unsigned char Image<unsigned char>::ValueMax = 255;

template <typename FormatT>
Image<FormatT>::Image()
	: m_data( nullptr ), m_width( 0 ), m_height( 0 ), m_numChan( 0 ), m_memAllocated( 0 )
{
}

template <typename FormatT>
Image<FormatT>::Image( int width, int height )
	: m_data( nullptr ), m_width( width ), m_height( height ), m_numChan( 0 ), m_memAllocated( 0 )
{ 
	std::cout << "Image.h: Image (w,h) constr." << std::endl; 
}

template <typename FormatT>
Image<FormatT>::~Image()
{ 
	delete[] m_data; std::cout << "Image.h: Image destr." << std::endl; 
}

/** ------------------------------------
*				  protected
*	------------------------------------
**/

template<typename FormatT>
FormatT Image<FormatT>::convertValue( FormatT value, FormatT )
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