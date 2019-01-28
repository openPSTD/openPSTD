#include <gtest/gtest.h>
#include "testcommand.h"
#include "../argumentparser.h"
#include "../createcommand.h"

constexpr double Settings::NEW_SCENE_GRID_SPACING;
const int Settings::NEW_SCENE_WINDOW_SIZE;
constexpr double Settings::NEW_SCENE_RENDER_TIME;
constexpr double Settings::NEW_SCENE_AIR_DENSITY;
const int Settings::NEW_SCENE_SOUND_SPEED;
const int Settings::NEW_SCENE_PML_CELLS;
const int Settings::NEW_SCENE_ATTENUATION;

/**
 * Verifies that the program exits with the correct error
 * message if the create command is called without a filename
 * option.
 */
TEST(CreateCommand, NoFilename) {
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD create");
	}, "\\[CreateCommand\\] Option \"-f\" is required\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the create command is called without a filename
 * behind the filename option.
 */
TEST(CreateCommand, EmptyFilename) {
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli create -f");
	}, "\\[CreateCommand\\] Option \"-f\" requires a filename\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the create command is called with a filename
 * that already exists.
 */
TEST(CreateCommand, FileExists) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this filename
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli create -f " + filename);
	}, "\\[CreateCommand\\] A file with this name already exists\n");
}

/**
 * Verifies that a scene file with the given name is created
 * if the program is used correctly.
 */
TEST(CreateCommand, CorrectUsage) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Execute the create command
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Assert that the scene file is created
	std::ifstream in(filename);
	ASSERT_TRUE(in.good());
	
	// Assert that the default values are set correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.domains.size(), 0);
	ASSERT_EQ(conf.sources.size(), 0);
	ASSERT_EQ(conf.receivers.size(), 0);
	ASSERT_EQ(conf.gridSpacing, Settings::NEW_SCENE_GRID_SPACING);
	ASSERT_EQ(conf.windowSize, Settings::NEW_SCENE_WINDOW_SIZE);
	ASSERT_EQ(conf.renderTime, Settings::NEW_SCENE_RENDER_TIME);
	ASSERT_EQ(conf.airDensity, Settings::NEW_SCENE_AIR_DENSITY);
	ASSERT_EQ(conf.soundSpeed, Settings::NEW_SCENE_SOUND_SPEED);
	ASSERT_EQ(conf.pmlCells, Settings::NEW_SCENE_PML_CELLS);
	ASSERT_EQ(conf.attenuation, Settings::NEW_SCENE_ATTENUATION);
}