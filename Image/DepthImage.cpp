#include "DepthImage.h"

#include <iostream>
#include <fstream>

DepthImage::DepthImage() : m_minDepth(0), m_maxDepth(1.0f)
{}


DepthImage::DepthImage(unsigned int width, unsigned int height) 
    : Image(width, height),
      m_minDepth(0),
      m_maxDepth(1.0f)
{}


DepthImage::~DepthImage()
{}


bool DepthImage::read(const std::string& fileName)
{
    std::ifstream infile;
    infile.open(fileName, std::ios::binary); 

     // if open failed
    if (!infile)
    {
        std::cerr << "Could not open file " << fileName << " for reading!"
                  << std::endl;
        return false;
    }

    infile.read((char*) &m_width, sizeof(m_width));
    infile.read((char*) &m_height, sizeof(m_height));
    m_data.resize(m_width * m_height);
    infile.read((char*) m_data.data(), sizeof(float) * size());

    infile.close();

    return true;
}


void DepthImage::write(const std::string& fileName) const
{
    std::ofstream outfile;

    outfile.open(fileName, std::ios::binary | std::ios::out);
    if (!outfile)
    {
        std::cerr << "Could not open file " << fileName << " for writing!"
                  << std::endl;
        return;
    }

    outfile.write((char*) &m_width, sizeof(m_width));
    outfile.write((char*) &m_height, sizeof(m_height));
    for (float value : m_data)
    {
        outfile.write((char*) &value, sizeof(float));
    }

    outfile.close();
}


float DepthImage::getDepthMin() const
{
    return m_minDepth;
}


float DepthImage::getDepthMax() const
{
    return m_maxDepth;
}


void DepthImage::setDepthMin(float min)
{
    m_minDepth = min;
}


void DepthImage::setDepthMax(float max)
{
    m_maxDepth = max;
}


ColorImage<float, ColorSpace::CS_GRAY> convertDepthToGray(const DepthImage& depthImg)
{
    ColorImage<float, ColorSpace::CS_GRAY> cImg(depthImg.getWidth(),
                                                depthImg.getHeight());
    for (int i = 0; i < depthImg.size(); ++i)
    {
        // Map meter to gray value.
        float depth = depthImg.getData()[i];
        depth = std::max(depthImg.getDepthMin(), depth);
        depth = std::min(depthImg.getDepthMax(), depth);

        depth = 1 - (depth - depthImg.getDepthMin()) / (depthImg.getDepthMax() - depthImg.getDepthMin());
        
        cImg.getData()[i] = depth;
    }
    return cImg;
}


DepthImage convertGrayToDepth(const ColorImage<float, ColorSpace::CS_GRAY>& grayImg)
{
    DepthImage depthImg(grayImg.getWidth(), grayImg.getHeight());

    for (int i = 0; i < grayImg.size(); ++i)
    {
        // Map gray value to meter
        float depth = grayImg.getData()[i];
        depth = std::max(0.0f, depth);
        depth = std::min(1.0f, depth);
        depthImg.getData()[i] = 1 - depth;
    }
    return depthImg;
}