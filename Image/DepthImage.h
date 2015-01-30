#pragma once
#include "Image.h"

template<typename T>
class DepthImage : public Image<T>
{
public:
	DepthImage(void);
	~DepthImage(void);
};

