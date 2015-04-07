#pragma once

#include "Image.h"
#include "ColorImage.h"


class DepthMap : public Image<float>
{
public:
    DepthMap();
    DepthMap(unsigned int width, unsigned int height);
    ~DepthMap();

    bool read(const std::string& fileName);
    void write(const std::string& fileName) const;

    float getDepthMin() const;
    float getDepthMax() const;
    void  setDepthMin(float min);
    void  setDepthMax(float max);

private:
    float m_minDepth, m_maxDepth; // The minimum and maximum values for distance (in meters).
};


ColorImage<float, ColorSpace::CS_GRAY> convertDepthToGray(const DepthMap& depthMap);