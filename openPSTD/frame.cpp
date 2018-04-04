#include "frame.h"

/**
 * Constructor.
 * Fills the domains vector with Frame_Domain instances.
 * 
 * @param frameID  The ID of the frame being represented
 */
Frame::Frame(int frameID) {
    // Loop through all domains
    for (unsigned int i = 0; i < 2; i++) {
        // Compute the filename of the data file outputted by the kernel
        std::string filename = "testdata/cpp_" + std::to_string(frameID) + "_domain_" + std::to_string(i) + ".csv";
        
        // Create a new Frame_Domain, and add it to the domains vector
        domains.push_back(new Frame_Domain(filename));
    }
}

/**
 * Destructor.
 */
Frame::~Frame() {
    // Delete all domains
    for (unsigned int i = 0; i < domains.size(); i++) {
        delete domains[i];
    }
}

/**
 * Constructor.
 * Reads the data file, and fills the samples vector.
 * 
 * @param filename  The filename of the data file to use
 */
Frame_Domain::Frame_Domain(std::string filename) {
    // Read the data file
    std::ifstream file(filename);
    std::string line;
    std::stringstream ss;
    
    // Loop through all lines in the file
    double v;
    width = 0;
    height = 0;
    while (std::getline(file, line)) {
        // Split line on comma
        if (line == "") continue;
        width = 0;
        height++;
        ss.str(line);
        ss.clear();
        while (ss >> v) {
            // Save the sample
            samples.push_back(v);
            
            // Ignore the comma
            if (ss.peek() == ',') ss.ignore();
            
            // Update width
            width++;
        }
    }
}

/**
 * Get method for a sample.
 * Note: The x and y coordinates are in kernel-grid coordinates,
 * and are offset by the domain's bottom-left corner.
 * 
 * @param x  The x-coordinate of the sample
 * @param y  The y-coordinate of the sample
 * @param domainID  The ID of the domain to check
 * @return  The sample value at the requested position
 */
double Frame::getSample(int x, int y, int domainID) {
    // Return 0 if there is no such domain
    if (domainID < 0 || domainID >= domains.size()) {
        return 0;
    }
    
    // Delegate to the corresponding domain
    return domains[domainID]->getSample(x, y);
}

/**
 * Get method for a sample.
 * Note: The x and y coordinates are in kernel-grid coordinates.
 * 
 * @param x  The x-coordinate of the sample
 * @param y  The y-coordinate of the sample
 * @return  The sample value at the requested position
 */
double Frame_Domain::getSample(int x, int y) {
    // Scale the coordinates to the kernel grid
    double gridSpacing = 0.2;
    int xx = x / gridSpacing;
    int yy = y / gridSpacing;
    
    // Verify that the sample exists
    int i = x + y * width;
    if (i < 0 || i >= samples.size()) {
        return 0;
    }
    
    // Return the value of the sample
    return samples[i];
}