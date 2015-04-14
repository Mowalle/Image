#include "DepthMap.h"

DepthMap::DepthMap() : m_minDepth(0), m_maxDepth(1.0f), m_meterToPixelScale(0)
{}


DepthMap::DepthMap(unsigned int width, unsigned int height) 
    : Image(width, height),
      m_minDepth(0),
      m_maxDepth(1.0f),
      m_meterToPixelScale(0)
{}


DepthMap::~DepthMap()
{}


bool DepthMap::read(const std::string& fileName)
{
    std::ifstream infile(fileName, std::ios::binary); 

     // if open failed
    if (!infile)
    {
        std::cerr << "Could not open file " << fileName << " for reading!"
                  << std::endl;
        return false;
    }

   
    infile.ignore( sizeof(int) ); // used to be number of channels

    infile.read( reinterpret_cast<char*>(&m_width), sizeof(m_width) );
    infile.read( reinterpret_cast<char*>(&m_height), sizeof(m_height) );
    m_data.resize(m_width * m_height);

    infile.ignore( sizeof(int) ); // used to be scale

    infile.read( reinterpret_cast<char*>(&m_meterToPixelScale), sizeof(m_meterToPixelScale) );
    infile.read( reinterpret_cast<char*>(m_data.data()), sizeof(float) * size() );

    infile.close();

    //TODO: Check for valid data
    return true;
}


void DepthMap::write(const std::string& fileName) const
{
    // TODO: Consider implementing a header info, something like in the .png format.

    std::ofstream outfile;

    outfile.open(fileName, std::ios::binary | std::ios::out);
    if (!outfile)
    {
        std::cerr << "Could not open file " << fileName << " for writing!"
                  << std::endl;
        return;
    }

    int placeholder = 1; // Used where before number of color channels and
                         // scale were written to file. Obsolete now.

    // Use of const_cast needed here, since we are in a const function.
    // Using reinterpret_cast insted of plain C-Cast.
    outfile.write(reinterpret_cast<char*>(&placeholder),
                  sizeof(int));
    outfile.write(reinterpret_cast<char*>(const_cast<unsigned*>(&m_width)),
                  sizeof(m_width));
    outfile.write(reinterpret_cast<char*>(const_cast<unsigned*>(&m_height)),
                  sizeof(m_height));
    outfile.write(reinterpret_cast<char*>(&placeholder),
                  sizeof(int)); // TODO: Implement scale
    outfile.write(reinterpret_cast<char*>(const_cast<float*>(&m_meterToPixelScale)),
                  sizeof(m_meterToPixelScale));
    outfile.write(reinterpret_cast<char*>(const_cast<float*>(m_data.data())),
                  sizeof(float) * size());

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


float DepthMap::getMeterToPixelScale() const
{
    return m_meterToPixelScale;
}


void DepthMap::setDepthMin(float min)
{
    m_minDepth = min;
}


void DepthMap::setDepthMax(float max)
{
    m_maxDepth = max;
}


void DepthMap::setMeterToPixelScale(float scale)
{
    m_meterToPixelScale = scale;
}


ColorImage<float, ColorSpace::CS_GRAY> convertDepthToGray(const DepthMap& depthMap)
{
    ColorImage<float, ColorSpace::CS_GRAY> cImg(depthMap.getWidth(), 
                                                depthMap.getHeight());
    
    // Map depth values to gray values.
    for (unsigned int i = 0; i < depthMap.size(); ++i)
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