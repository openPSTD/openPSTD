#ifndef FILEIO_H
#define FILEIO_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "configuration.h"

/**
 * Handles all file operations.
 */
class FileIO {
public:
	// Reads a given scene file.
	static Configuration readSceneFile(std::string filename);
	
	// Saves a configuration into a scene file.
	static void saveSceneFile(std::string filename, Configuration conf);
	
	// Saves a single frame into a frame file.
	static void saveFrameFile(std::string filename, Configuration* conf);
private:
	// Private stringstream parsing methods.
	static int getInt(std::stringstream* ss);
	static double getDouble(std::stringstream* ss);
	static bool getBool(std::stringstream* ss);
};

#endif