#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

/**
 * Represents a single domain inside a single frame.
 */
class Frame_Domain {
public:
    // Constructor
    Frame_Domain(std::string filename);
    
    // Get method for a sample
    double getSample(int x, int y);
    
    // Data vector
    std::vector<double> samples;
    
    // Width and height of the domain according to the kernel grid
    int width;
    int height;
};

/**
 * Represents a single simulated frame across all domains.
 */
class Frame {
public:
    // Constructor, destructor
    Frame(int frameID, unsigned int numdomains);
    
    // Get method for a sample
    double getSample(int x, int y, int domainID);
    
    // Get method for the width and height of the domains
    int getWidth(int domainID) { return domains[domainID].width; }
    int getHeight(int domainID) { return domains[domainID].height; }
private:
    // Data vector
    std::vector<Frame_Domain> domains;
    
    // Fills the domains vector with Frame_Domain instances
    void readDomainFile(std::string filename, int i);
};

#endif