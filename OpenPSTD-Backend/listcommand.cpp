#include "listcommand.h"

/**
 * Executes the list command.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void ListCommand::execute(ArgumentParser* parser) {
	// Verify that all required arguments are supplied
	std::string filename;
	if (!parser->hasOption("-f", &filename)) {
		std::cerr << "[ListCommand] Option \"-f\" is required" << std::endl;
		exit(1);
	}
	if (filename == "") {
		std::cerr << "[ListCommand] Option \"-f\" requires a filename" << std::endl;
		exit(1);
	}
	
	// Read the supplied scene file
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Print all domains
	for (unsigned int i = 0; i < conf.domains.size(); i++) {
		std::cout << "domain " << i << std::endl;
		std::cout << "  left: " << conf.domains[i].left << std::endl;
		std::cout << "  top: " << conf.domains[i].top << std::endl;
		std::cout << "  width: " << conf.domains[i].width << std::endl;
		std::cout << "  height: " << conf.domains[i].height << std::endl;
		std::cout << "  absorption top: " << conf.domains[i].absorptionTop << std::endl;
		std::cout << "  absorption bottom: " << conf.domains[i].absorptionBottom << std::endl;
		std::cout << "  absorption left: " << conf.domains[i].absorptionLeft << std::endl;
		std::cout << "  absorption right: " << conf.domains[i].absorptionRight << std::endl;
		std::cout << "  ELR top: " << conf.domains[i].elrTop << std::endl;
		std::cout << "  ELR bottom: " << conf.domains[i].elrBottom << std::endl;
		std::cout << "  ELR left: " << conf.domains[i].elrLeft << std::endl;
		std::cout << "  ELR right: " << conf.domains[i].elrRight << std::endl;
	}
	
	// Print all sources
	for (unsigned int i = 0; i < conf.sources.size(); i++) {
		std::cout << "source " << i << ": [";
		std::cout << conf.sources[i].position_x << ", ";
		std::cout << conf.sources[i].position_y << "]" << std::endl;
	}
	
	// Print all receivers
	for (unsigned int i = 0; i < conf.receivers.size(); i++) {
		std::cout << "receiver " << i << ": [";
		std::cout << conf.receivers[i].position_x << ", ";
		std::cout << conf.receivers[i].position_y << "]" << std::endl;
	}
	
	// Print the other settings
	std::cout << "gridSpacing: " << conf.gridSpacing << std::endl;
	std::cout << "windowSize: " << conf.windowSize << std::endl;
	std::cout << "renderTime: " << conf.renderTime << std::endl;
	std::cout << "airDensity: " << conf.airDensity << std::endl;
	std::cout << "soundSpeed: " << conf.soundSpeed << std::endl;
	std::cout << "pmlCells: " << conf.pmlCells << std::endl;
	std::cout << "attenuation: " << conf.attenuation << std::endl;
}