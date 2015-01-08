#pragma once

#include "Image.h"

template <typename T>
class IntegralImage : public Image<T>
{
public:
	IntegralImage(void);
	~IntegralImage(void);
};

