#include "frame.h"

Frame::Frame(std::string filename) {
    // Read the data file
    std::ifstream file(filename);
    std::string line;
    std::stringstream ss;
    
    // Loop through all lines in the file
    /*double v;
    while (std::getline(file, line)) {
        // Split line on comma
        width = 0;
        while (ss >> v) {
            // Save the sample
            samples.push_back(v);
            
            // Ignore the comma
            if (ss.peek() == ',') ss.ignore();
            
            // Update width
            width++;
        }
    }*/
    width = 0;
}

double Frame::getSample(int x, int y) {
    // Verify that the sample exists
    int i = x + y * width;
    if (i < 0 || i >= samples.size()) return 0;
    
    // Return the value of the sample
    return samples[i];
}