#include "editcommand.h"

/**
 * Executes the edit command.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::execute(ArgumentParser* parser) {
	// Verify that all required arguments are supplied
	std::string filename = "";
	if (!parser->hasOption("-f", &filename)) {
		std::cerr << "[EditCommand] Option \"-f\" is required" << std::endl;
		exit(1);
	}
	if (filename == "") {
		std::cerr << "[EditCommand] Option \"-f\" requires a filename" << std::endl;
		exit(1);
	}
	
	// Delegate based on the supplied arguments
	if (parser->hasOption("-d", nullptr)) addDomain(parser);
	if (parser->hasOption("-D", nullptr)) removeDomain(parser);
	if (parser->hasOption("-a", nullptr)) setAbsorption(parser);
	if (parser->hasOption("-l", nullptr)) setELR(parser);
	if (parser->hasOption("-p", nullptr)) addSource(parser);
	if (parser->hasOption("-P", nullptr)) removeSource(parser);
	if (parser->hasOption("-r", nullptr)) addReceiver(parser);
	if (parser->hasOption("-R", nullptr)) removeReceiver(parser);
	if (parser->hasOption("--grid-spacing", nullptr)) setGridSpacing(parser);
	if (parser->hasOption("--window-size", nullptr)) setWindowSize(parser);
	if (parser->hasOption("--render-time", nullptr)) setRenderTime(parser);
	if (parser->hasOption("--density-of-air", nullptr)) setAirDensity(parser);
	if (parser->hasOption("--sound-speed", nullptr)) setSoundSpeed(parser);
	if (parser->hasOption("--pml-cells", nullptr)) setPMLCells(parser);
	if (parser->hasOption("--attenuation-of-pml-cells", nullptr)) setAttenuation(parser);
}

/**
 * Private editing method: Add a domain.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::addDomain(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the domain location argument
	std::string location;
	if (!parser->hasOption("-d", &location)) return;
	std::vector<std::string> coords = parseSegments(location);
	if (coords.size() != 4) {
		std::cerr << "[EditCommand] Parse error in location" << std::endl;
		exit(1);
	}
	
	// Add a new domain
	Domain d = {
		std::atoi(coords[0].c_str()), std::atoi(coords[1].c_str()),
		std::atoi(coords[2].c_str()), std::atoi(coords[3].c_str()),
		Settings::NEW_DOMAIN_ABSORPTION_TOP, Settings::NEW_DOMAIN_ABSORPTION_BOTTOM,
		Settings::NEW_DOMAIN_ABSORPTION_LEFT, Settings::NEW_DOMAIN_ABSORPTION_RIGHT,
		Settings::NEW_DOMAIN_ELR_TOP, Settings::NEW_DOMAIN_ELR_BOTTOM,
		Settings::NEW_DOMAIN_ELR_LEFT, Settings::NEW_DOMAIN_ELR_RIGHT,
		false
	};
	conf.domains.push_back(d);
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Remove a domain.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::removeDomain(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the domain ID argument
	std::string idstr;
	if (!parser->hasOption("-D", &idstr)) return;
	unsigned int id = static_cast<unsigned int>(std::atoi(idstr.c_str()));
	if (id >= conf.domains.size()) {
		std::cerr << "[EditCommand] Domain id \"" << id << "\" does not exist" << std::endl;
		exit(1);
	}
	
	// Remove the domain with given ID
	conf.domains.erase(conf.domains.begin() + id);
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Set domain absorption.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::setAbsorption(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the absorption string
	std::string absorptionstr;
	if (!parser->hasOption("-a", &absorptionstr)) return;
	std::vector<std::string> segments = parseSegments(absorptionstr);
	if (segments.size() != 3) {
		std::cerr << "[EditCommand] Parse error in absorption" << std::endl;
		exit(1);
	}
	
	// First element is the domain ID
	unsigned int id = static_cast<unsigned int>(std::atoi(segments[0].c_str()));
	if (id >= conf.domains.size()) {
		std::cerr << "[EditCommand] Domain id \"" << id << "\" does not exist" << std::endl;
		exit(1);
	}
	
	// Second element is the wall side
	if (segments[1].size() != 1) {
		std::cerr << "[EditCommand] Parse error in absorption" << std::endl;
		exit(1);
	}
	char side = segments[1][0];
	if (side != 't' && side != 'b' && side != 'l' && side != 'r') {
		std::cerr << "[EditCommand] Parse error in absorption" << std::endl;
		exit(1);
	}
	
	// Third element is the absorption value
	double absorption = std::atof(segments[2].c_str());
	
	// Set the domain's absorption value
	if (side == 't') conf.domains[id].absorptionTop = absorption;
	if (side == 'b') conf.domains[id].absorptionBottom = absorption;
	if (side == 'l') conf.domains[id].absorptionLeft = absorption;
	if (side == 'r') conf.domains[id].absorptionRight = absorption;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Set domain ELR.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::setELR(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the ELR string
	std::string elrstr;
	if (!parser->hasOption("-l", &elrstr)) return;
	std::vector<std::string> segments = parseSegments(elrstr);
	if (segments.size() != 3) {
		std::cerr << "[EditCommand] Parse error in ELR" << std::endl;
		exit(1);
	}
	
	// First element is the domain ID
	unsigned int id = static_cast<unsigned int>(std::atoi(segments[0].c_str()));
	if (id >= conf.domains.size()) {
		std::cerr << "[EditCommand] Domain id \"" << id << "\" does not exist" << std::endl;
		exit(1);
	}
	
	// Second element is the wall side
	if (segments[1].size() != 1) {
		std::cerr << "[EditCommand] Parse error in ELR" << std::endl;
		exit(1);
	}
	char side = segments[1][0];
	if (side != 't' && side != 'b' && side != 'l' && side != 'r') {
		std::cerr << "[EditCommand] Parse error in ELR" << std::endl;
		exit(1);
	}
	
	// Third element is the elr value
	if (segments[2] != "0" && segments[2] != "1") {
		std::cerr << "[EditCommand] Parse error in ELR" << std::endl;
		exit(1);
	}
	bool elr = (segments[2] == "1");
	
	// Set the domain's ELR value
	if (side == 't') conf.domains[id].elrTop = elr;
	if (side == 'b') conf.domains[id].elrBottom = elr;
	if (side == 'l') conf.domains[id].elrLeft = elr;
	if (side == 'r') conf.domains[id].elrRight = elr;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Add a source.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::addSource(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the location string
	std::string locationstr;
	if (!parser->hasOption("-p", &locationstr)) return;
	std::vector<std::string> segments = parseSegments(locationstr);
	if (segments.size() != 2) {
		std::cerr << "[EditCommand] Parse error in location" << std::endl;
		exit(1);
	}
	
	// Add a new source
	Source s = {
		std::atoi(segments[0].c_str()),
		std::atoi(segments[1].c_str())
	};
	conf.sources.push_back(s);
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Remove a source.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::removeSource(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the source ID argument
	std::string idstr;
	if (!parser->hasOption("-P", &idstr)) return;
	unsigned int id = static_cast<unsigned int>(std::atoi(idstr.c_str()));
	if (id >= conf.sources.size()) {
		std::cerr << "[EditCommand] Source id \"" << id << "\" does not exist" << std::endl;
		exit(1);
	}
	
	// Remove the source with given ID
	conf.sources.erase(conf.sources.begin() + id);
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Add a receiver.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::addReceiver(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the location string
	std::string locationstr;
	if (!parser->hasOption("-r", &locationstr)) return;
	std::vector<std::string> segments = parseSegments(locationstr);
	if (segments.size() != 2) {
		std::cerr << "[EditCommand] Parse error in location" << std::endl;
		exit(1);
	}
	
	// Add a new receiver
	Receiver r = {
		std::atoi(segments[0].c_str()),
		std::atoi(segments[1].c_str())
	};
	conf.receivers.push_back(r);
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Remove a receiver.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::removeReceiver(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the receiver ID argument
	std::string idstr;
	if (!parser->hasOption("-R", &idstr)) return;
	unsigned int id = static_cast<unsigned int>(std::atoi(idstr.c_str()));
	if (id >= conf.receivers.size()) {
		std::cerr << "[EditCommand] Receiver id \"" << id << "\" does not exist" << std::endl;
		exit(1);
	}
	
	// Remove the receiver with given ID
	conf.receivers.erase(conf.receivers.begin() + id);
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Set the grid spacing.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::setGridSpacing(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the grid spacing argument
	std::string valuestr;
	if (!parser->hasOption("--grid-spacing", &valuestr)) return;
	double value = std::atof(valuestr.c_str());
	
	// Set the grid spacing
	conf.gridSpacing = value;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Set the window size.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::setWindowSize(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the window size argument
	std::string valuestr;
	if (!parser->hasOption("--window-size", &valuestr)) return;
	int value = std::atoi(valuestr.c_str());
	
	// Set the window size
	conf.windowSize = value;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Set the render time.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::setRenderTime(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the render time argument
	std::string valuestr;
	if (!parser->hasOption("--render-time", &valuestr)) return;
	double value = std::atof(valuestr.c_str());
	
	// Set the render time
	conf.renderTime = value;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Set the air density.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::setAirDensity(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the air density argument
	std::string valuestr;
	if (!parser->hasOption("--density-of-air", &valuestr)) return;
	double value = std::atof(valuestr.c_str());
	
	// Set the air density
	conf.airDensity = value;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Set the sound speed.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::setSoundSpeed(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the sound speed argument
	std::string valuestr;
	if (!parser->hasOption("--sound-speed", &valuestr)) return;
	int value = std::atoi(valuestr.c_str());
	
	// Set the sound speed
	conf.soundSpeed = value;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Set the PML cells.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void EditCommand::setPMLCells(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the pml cells argument
	std::string valuestr;
	if (!parser->hasOption("--pml-cells", &valuestr)) return;
	int value = std::atoi(valuestr.c_str());
	
	// Set the pml cells
	conf.pmlCells = value;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Private editing method: Set the attenuation of the pml cells.
 * 
 * @param parser  A reference to the supplied arguments
 */
void EditCommand::setAttenuation(ArgumentParser* parser) {
	// Read the scene file
	std::string filename;
	if (!parser->hasOption("-f", &filename)) return;
	Configuration conf = FileIO::readSceneFile(filename);
	
	// Parse the attenuation argument
	std::string valuestr;
	if (!parser->hasOption("--attenuation-of-pml-cells", &valuestr)) return;
	int value = std::atoi(valuestr.c_str());
	
	// Set the attenuation
	conf.attenuation = value;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Parses a string in the form "[one,two,...]" into a vector of substrings.
 * 
 * @param s  The string to parse
 * @return  A vector of the parsed segments
 */
std::vector<std::string> EditCommand::parseSegments(std::string s) {
	// Verify that the given string begins with '[' and ends with ']'
	if (s[0] != '[' || s.back() != ']') {
		std::cerr << "[EditCommand] Parse error in coordinates" << std::endl;
		exit(1);
	}
	
	// Extract the substring between the '[' and ']' characters
	std::string sub = s.substr(1, s.size() - 2);
	std::stringstream ss(sub);
	
	// Split the substring on commas
	std::vector<std::string> result;
	std::string segment;
	while (std::getline(ss, segment, ',')) {
		result.push_back(segment);
	}
	
	// Return the result vector
	return result;
}