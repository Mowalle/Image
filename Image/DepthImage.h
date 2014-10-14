#pragma once
#include "Image.h"

template <typename Type>
class DepthImage : public Image<Type>
{
public:
	DepthImage(void);
	~DepthImage(void);
};

