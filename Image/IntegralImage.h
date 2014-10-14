#pragma once

#include "Image.h"

template <typename Type>
class IntegralImage : public Image<Type>
{
public:
	IntegralImage(void);
	~IntegralImage(void);
};

