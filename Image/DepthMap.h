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
    float getMeterToPixelScale() const;

    void setDepthMin(float min);
    void setDepthMax(float max);
    void setMeterToPixelScale(float scale);

private:
    float m_minDepth, m_maxDepth; // The minimum and maximum values for distance (in meters).
    float m_meterToPixelScale;
};


ColorImage<float, ColorSpace::CS_GRAY> convertDepthToGray(const DepthMap& depthMap);