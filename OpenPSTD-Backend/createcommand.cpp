#include "createcommand.h"

/**
 * Executes the create command.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void CreateCommand::execute(ArgumentParser* parser) {
	// Verify that all required arguments are supplied
	std::string filename = "";
	if (!parser->hasOption("-f", &filename)) {
		std::cerr << "[CreateCommand] Option \"-f\" is required" << std::endl;
		exit(1);
	}
	if (filename == "") {
		std::cerr << "[CreateCommand] Option \"-f\" requires a filename" << std::endl;
		exit(1);
	}
	
	// Verify that no file exists already with this name
	std::ifstream in(filename.c_str());
	if (in.good()) {
		std::cerr << "[CreateCommand] A file with this name already exists" << std::endl;
		exit(1);
	}
	
	// Create a default Configuration instance
	Configuration conf = getDefaultConfiguration();
	
	// Save the default configuration to a file with given name
	FileIO::saveSceneFile(filename, conf);
}

/**
 * Creates a default Configuration instance.
 * 
 * @return  The created Configuration instance
 */
Configuration CreateCommand::getDefaultConfiguration() {
	// Initialize the result instance
	Configuration conf;
	
	// Set the default values
	conf.gridSpacing = Settings::NEW_SCENE_GRID_SPACING;
	conf.windowSize = Settings::NEW_SCENE_WINDOW_SIZE;
	conf.renderTime = Settings::NEW_SCENE_RENDER_TIME;
	conf.airDensity = Settings::NEW_SCENE_AIR_DENSITY;
	conf.soundSpeed = Settings::NEW_SCENE_SOUND_SPEED;
	conf.pmlCells = Settings::NEW_SCENE_PML_CELLS;
	conf.attenuation = Settings::NEW_SCENE_ATTENUATION;
	conf.saventhframe = Settings::NEW_SCENE_SAVENTHFRAME;
	
	// Return the result instance
	return conf;
}