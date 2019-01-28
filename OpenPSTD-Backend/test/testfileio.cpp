#include <gtest/gtest.h>
#include "testcommand.h"
#include "../fileio.h"

/**
 * Verifies that the program exits with the correct error
 * message if the given file does not exist.
 */
TEST(FileIO, ReadSceneFileNonExistingFile) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		FileIO::readSceneFile(filename);
	}, "\\[FileIO\\] Could not read scene file \"" + filename + "\"\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the given file contains an unknown data type.
 */
TEST(FileIO, ReadSceneFileUnknownDataType) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Write some unknown datatype to the file
	std::ofstream out(filename.c_str());
	out << "mykey,myvalue\n";
	out.close();
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		FileIO::readSceneFile(filename);
	}, "\\[FileIO\\] Unknown data type \"mykey\"\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the scene file cannot be written.
 * Implementation: Attempting to open a directory for writing
 * (e.g. "CMakeFiles"), should result in the filestream failing.
 */
TEST(FileIO, SaveSceneFileNoWritePossible) {
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		Configuration conf;
		FileIO::saveSceneFile("CMakeFiles", conf);
	}, "\\[FileIO\\] Could not write scene file \"CMakeFiles\"\n");
}

/**
 * Verifies that a scene file can be saved and read, and
 * still contains the same data as before saving it.
 */
TEST(FileIO, SaveReadSceneFileCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Setup a default configuration
	Configuration conf;
	conf.gridSpacing = Settings::NEW_SCENE_GRID_SPACING;
	conf.windowSize = Settings::NEW_SCENE_WINDOW_SIZE;
	conf.renderTime = Settings::NEW_SCENE_RENDER_TIME;
	conf.airDensity = Settings::NEW_SCENE_AIR_DENSITY;
	conf.soundSpeed = Settings::NEW_SCENE_SOUND_SPEED;
	conf.pmlCells = Settings::NEW_SCENE_PML_CELLS;
	conf.attenuation = Settings::NEW_SCENE_ATTENUATION;
	
	// Save the scene file
	FileIO::saveSceneFile(filename, conf);
	
	// Read the scene file
	Configuration conf2 = FileIO::readSceneFile(filename);
	
	// Assert that all values in the two configurations are the same
	ASSERT_EQ(conf.gridSpacing, conf2.gridSpacing);
	ASSERT_EQ(conf.windowSize, conf2.windowSize);
	ASSERT_EQ(conf.renderTime, conf2.renderTime);
	ASSERT_EQ(conf.airDensity, conf2.airDensity);
	ASSERT_EQ(conf.soundSpeed, conf2.soundSpeed);
	ASSERT_EQ(conf.pmlCells, conf2.pmlCells);
	ASSERT_EQ(conf.attenuation, conf2.attenuation);
}

/**
 * Verifies that the program exits with the correct error
 * message if getInt() attempts to parse an empty string.
 */
TEST(FileIO, GetIntEmpty) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Write some data that would cause getInt() to parse an empty string
	std::ofstream out(filename.c_str());
	out << "windowsize,\n";
	out.close();
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		FileIO::readSceneFile(filename);
	}, "\\[FileIO\\] Parse exception\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if getDouble() attempts to parse an empty string.
 */
TEST(FileIO, GetDoubleEmpty) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Write some data that would cause getInt() to parse an empty string
	std::ofstream out(filename.c_str());
	out << "gridspacing,\n";
	out.close();
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		FileIO::readSceneFile(filename);
	}, "\\[FileIO\\] Parse exception\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if getBool() attempts to parse an empty string.
 */
TEST(FileIO, GetBoolEmpty) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Write some data that would cause getInt() to parse an empty string
	std::ofstream out(filename.c_str());
	out << "domain,1,2,3,4,0,0,0,0,,,,\n";
	out.close();
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		FileIO::readSceneFile(filename);
	}, "\\[FileIO\\] Parse exception\n");
}