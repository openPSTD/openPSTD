#include "fileio.h"

/**
 * Reads a given scene file.
 * 
 * @param filename  The filename of the scene file to read
 * @return  A Configuration instance for the requested scene file
 */
Configuration FileIO::readSceneFile(std::string filename) {
	// Open the scene file for reading
	std::ifstream in(filename.c_str());
	if (!in.good()) {
		std::cerr << "[FileIO] Could not read scene file \"" << filename << "\"" << std::endl;
		exit(1);
	}
	
	// Initialize the result configuration
	Configuration conf;
	
	// Loop through all lines in the file
	std::string line;
	while (std::getline(in, line)) {
		// Split the line on comma
		if (line == "") continue;
		std::stringstream ss(line);
		
		// First element is the data type
		std::string type;
		std::getline(ss, type, ',');
		if (type == "domain") {
			// Format: "domain,<left>,<top>,<width>,<height>,<absorptionTop>,
			// <absorptionBottom>,<absorptionLeft>,<absorptionRight>,<elrTop>,
			// <elrBottom>,<elrLeft>,<elrRight>"
			int left = getInt(&ss);
			int top = getInt(&ss);
			int width = getInt(&ss);
			int height = getInt(&ss);
			double absorptionTop = getDouble(&ss);
			double absorptionBottom = getDouble(&ss);
			double absorptionLeft = getDouble(&ss);
			double absorptionRight = getDouble(&ss);
			bool elrTop = getBool(&ss);
			bool elrBottom = getBool(&ss);
			bool elrLeft = getBool(&ss);
			bool elrRight = getBool(&ss);
			conf.domains.push_back({
				left, top, width, height,
				absorptionTop, absorptionBottom, absorptionLeft, absorptionRight,
				elrTop, elrBottom, elrLeft, elrRight,
				false
			});
			continue;
		}
		if (type == "source") {
			// Format: "source,<x>,<y>"
			int x = getInt(&ss);
			int y = getInt(&ss);
			conf.sources.push_back({x, y});
			continue;
		}
		if (type == "receiver") {
			// Format: "receiver,<x>,<y>"
			int x = getInt(&ss);
			int y = getInt(&ss);
			conf.receivers.push_back({x, y});
			continue;
		}
		if (type == "gridspacing") {
			// Format: "gridspacing,<value>"
			double gridSpacing = getDouble(&ss);
			conf.gridSpacing = gridSpacing;
			continue;
		}
		if (type == "windowsize") {
			// Format: "windowsize,<value>"
			int windowSize = getInt(&ss);
			conf.windowSize = windowSize;
			continue;
		}
		if (type == "rendertime") {
			// Format: "rendertime,<value>"
			double renderTime = getDouble(&ss);
			conf.renderTime = renderTime;
			continue;
		}
		if (type == "airdensity") {
			// Format: "airdensity,<value>"
			double airDensity = getDouble(&ss);
			conf.airDensity = airDensity;
			continue;
		}
		if (type == "soundspeed") {
			// Format: "soundspeed,<value>"
			int soundSpeed = getInt(&ss);
			conf.soundSpeed = soundSpeed;
			continue;
		}
		if (type == "pmlcells") {
			// Format: "pmlcells,<value>"
			int pmlCells = getInt(&ss);
			conf.pmlCells = pmlCells;
			continue;
		}
		if (type == "attenuation") {
			// Format: "attenuation,<value>"
			int attenuation = getInt(&ss);
			conf.attenuation = attenuation;
			continue;
		}
		
		// Unknown data type
		std::cerr << "[FileIO] Unknown data type \"" << type << "\"" << std::endl;
		exit(1);
	}
	
	// Return the parsed file
	return conf;
}

/**
 * Saves a configuration into a scene file.
 * 
 * @param filename  The filename of the scene file to save to
 * @param conf  A Configuration instance to save to the given file
 */
void FileIO::saveSceneFile(std::string filename, Configuration conf) {
	// Open the scene file for writing
	std::ofstream out(filename.c_str());
	if (!out.good()) {
		std::cerr << "[FileIO] Could not write scene file \"" << filename << "\"" << std::endl;
		exit(1);
	}
	
	// Write all domains
	for (unsigned int i = 0; i < conf.domains.size(); i++) {
		if (conf.domains[i].isPML) continue;
		out << "domain,";
		out << conf.domains[i].left << ",";
		out << conf.domains[i].top << ",";
		out << conf.domains[i].width << ",";
		out << conf.domains[i].height << ",";
		out << conf.domains[i].absorptionTop << ",";
		out << conf.domains[i].absorptionBottom << ",";
		out << conf.domains[i].absorptionLeft << ",";
		out << conf.domains[i].absorptionRight << ",";
		out << conf.domains[i].elrTop << ",";
		out << conf.domains[i].elrBottom << ",";
		out << conf.domains[i].elrLeft << ",";
		out << conf.domains[i].elrRight << std::endl;
	}
	
	// Write all sources
	for (unsigned int i = 0; i < conf.sources.size(); i++) {
		out << "source,";
		out << conf.sources[i].position_x << ",";
		out << conf.sources[i].position_y << std::endl;
	}
	
	// Write all receivers
	for (unsigned int i = 0; i < conf.receivers.size(); i++) {
		out << "receiver,";
		out << conf.receivers[i].position_x << ",";
		out << conf.receivers[i].position_y << std::endl;
	}
	
	// Write the other settings variables
	out << "gridspacing," << conf.gridSpacing << std::endl;
	out << "windowsize," << conf.windowSize << std::endl;
	out << "rendertime," << conf.renderTime << std::endl;
	out << "airdensity," << conf.airDensity << std::endl;
	out << "soundspeed," << conf.soundSpeed << std::endl;
	out << "pmlcells," << conf.pmlCells << std::endl;
	out << "attenuation," << conf.attenuation << std::endl;
}

/**
 * Saves a single frame into a frame file.
 * 
 * @param filename  The filename of the frame file to save to
 * @param conf  A reference to a Configuration instance containing the frame data
 */
void FileIO::saveFrameFile(std::string filename, Configuration* conf) {}

/**
 * Parses the next comma-separated word in the given stringstream
 * into an integer.
 * 
 * @param ss  A reference to the stringstream to parse from
 * @return  The parsed integer
 */
int FileIO::getInt(std::stringstream* ss) {
	std::string str;
	std::getline(*ss, str, ',');
	if (str == "") {
		std::cerr << "[FileIO] Parse exception" << std::endl;
		exit(1);
	}
	return std::atoi(str.c_str());
}

/**
 * Parses the next comma-separated word in the given stringstream
 * into a double.
 * 
 * @param ss  A reference to the stringstream to parse from
 * @return  The parsed double
 */
double FileIO::getDouble(std::stringstream* ss) {
	std::string str;
	std::getline(*ss, str, ',');
	if (str == "") {
		std::cerr << "[FileIO] Parse exception" << std::endl;
		exit(1);
	}
	return std::atof(str.c_str());
}

/**
 * Parses the next comma-separated word in the given stringstream
 * into a boolean.
 * 
 * @param ss  A reference to the stringstream to parse from
 * @return  The parsed boolean
 */
bool FileIO::getBool(std::stringstream* ss) {
	std::string str;
	std::getline(*ss, str, ',');
	if (str != "1" && str != "0") {
		std::cerr << "[FileIO] Parse exception" << std::endl;
		exit(1);
	}
	return (str == "1");
}