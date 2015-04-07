#include "DepthMap.h"

#include <iostream>
#include <fstream>


DepthMap::DepthMap() : m_minDepth(0), m_maxDepth(1.0f)
{}


DepthMap::DepthMap(unsigned int width, unsigned int height) 
    : Image(width, height),
      m_minDepth(0),
      m_maxDepth(1.0f)
{}


DepthMap::~DepthMap()
{}


bool DepthMap::read(const std::string& fileName)
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

    // Check for legacy format.
    infile.ignore(sizeof(int)); // used to be number of channels
    infile.read((char*) &m_width, sizeof(m_width));
    infile.read((char*) &m_height, sizeof(m_height));
    infile.ignore(sizeof(int)); // used to be scale
    infile.ignore(sizeof(int)); // used to be meter2Pixel

    m_data.resize(m_width * m_height);
    infile.read((char*) m_data.data(), sizeof(float) * size());


    infile.close();

    //TODO: Check for valid data
    return true;
}


void DepthMap::write(const std::string& fileName) const
{
    // TODO: Consider implemeting a header info, something like in the .png format.

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


float DepthMap::getDepthMin() const
{
    return m_minDepth;
}


float DepthMap::getDepthMax() const
{
    return m_maxDepth;
}


void DepthMap::setDepthMin(float min)
{
    m_minDepth = min;
}


void DepthMap::setDepthMax(float max)
{
    m_maxDepth = max;
}


ColorImage<float, ColorSpace::CS_GRAY> convertDepthToGray(const DepthMap& depthMap)
{
    ColorImage<float, ColorSpace::CS_GRAY> cImg(depthMap.getWidth(), 
                                                depthMap.getHeight());
    
    // Map depth values to gray values.
    for (int i = 0; i < depthMap.size(); ++i)
    {
        float depth = depthMap.getData()[i];
        // Keep value in range min, max.
        depth = std::max(depthMap.getDepthMin(), depth);
        depth = std::min(depthMap.getDepthMax(), depth);

        // Actual mapping. 
        depth = 1 - (depth - depthMap.getDepthMin()) / (depthMap.getDepthMax() - depthMap.getDepthMin());
        
        // Keep in float range. Just in case.
        cImg.getData()[i] = std::max(0.0f, std::min(depth, 1.0f));
    }
    return cImg;
}