#include <gtest/gtest.h>
#include "testcommand.h"
#include "../runcommand.h"

/**
 * Verifies that the program exits with the correct error
 * message if the run command is called without a filename
 * option.
 */
TEST(RunCommand, NoFilename) {
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli run");
	}, "\\[RunCommand\\] Option \"-f\" is required\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the run command is called without a filename
 * behind the filename option.
 */
TEST(RunCommand, EmptyFilename) {
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli run -f");
	}, "\\[RunCommand\\] Option \"-f\" requires a filename\n");
}

/**
 * Verifies that a simulation is ran, and that frame files
 * are being written when the run command is used correctly.
 */
TEST(RunCommand, Correct) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this filename
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Execute the run command
	testing::internal::CaptureStdout();
	executeCommand("./OpenPSTD-cli run -f " + filename);
	std::string output = testing::internal::GetCapturedStdout();
	
	// Compute the number of frames that should have been computed
	Configuration conf = FileIO::readSceneFile(filename);
	Solver solver(&conf);
	unsigned int numFrames = solver.getNumFrames();
	
	// Assert that the intermediate frame finished strings are printed to stdout
	ASSERT_NE(output.find("Finished frame: 0"), std::string::npos);
	for (unsigned int i = 1; i < numFrames; i++) {
		ASSERT_NE(output.find("\nFinished frame: " + std::to_string(i)), std::string::npos);
	}
	
	// Assert that the final finished simulation string is printed to stdout
	ASSERT_NE(output.find("\nSuccessfully finished simulation"), std::string::npos);
	
	// Assert that all frame files have been written
	for (unsigned int i = 0; i < numFrames; i++) {
		// TODO: Verify that the frame file exists
	}
}