#include "frame.h"

/**
 * Constructor.
 * Fills the domains vector with Frame_Domain instances.
 * 
 * @param frameID  The ID of the frame being represented
 * @param numdomains  The number of domains in the frame
 */
Frame::Frame(int frameID, unsigned int numdomains) {
	// Loop through all domains
	for (unsigned int i = 0; i < numdomains; i++) {
		// Compute the filename of the data file outputted by the kernel
		std::stringstream ss;
		ss << "testdata/cpp_" << frameID << "_domain_" << i << ".csv";
		std::string filename = ss.str();
		
		// Create a new Frame_Domain, and add it to the domains vector
		domains.push_back(Frame_Domain(filename));
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
double Frame::getSample(unsigned int x, unsigned int y, unsigned int domainID) {
	// Return 0 if there is no such domain
	if (domainID >= domains.size()) {
		return 0;
	}
	
	// Delegate to the corresponding domain
	return domains[domainID].getSample(x, y);
}

/**
 * Get method for a sample.
 * Note: The x and y coordinates are in kernel-grid coordinates.
 * 
 * @param x  The x-coordinate of the sample
 * @param y  The y-coordinate of the sample
 * @return  The sample value at the requested position
 */
double Frame_Domain::getSample(unsigned int x, unsigned int y) {
	// Verify that the sample exists
	unsigned int i = x + y * width;
	if (i >= samples.size()) {
		return 0;
	}
	
	// Return the value of the sample
	return samples[i];
}
