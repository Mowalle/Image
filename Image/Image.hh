/**
*	This file must be excluded from build!
**/



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