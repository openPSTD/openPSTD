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
 * Verifies that the program exits with the correct error
 * message if the run command is called with a scene file
 * that does not contain any domains.
 */
TEST(RunCommand, NoDomains) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this filename
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Execute the run command
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli run -f " + filename);
	}, "\\[SceneValidator\\] There must be at least one domain\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the run command is called with a scene file
 * that does not contain any sources.
 */
TEST(RunCommand, NoSources) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this filename that should trigger the error
	executeCommand("./OpenPSTD-cli create -f " + filename);
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Execute the run command
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli run -f " + filename);
	}, "\\[SceneValidator\\] There must be at least one source\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the run command is called with a scene file
 * that contains disconnected domains.
 */
TEST(RunCommand, DisconnectedDomains) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this filename that should trigger the error
	executeCommand("./OpenPSTD-cli create -f " + filename);
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -d [11,12,3,4] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -p [2,3] -f " + filename);
	
	// Execute the run command
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli run -f " + filename);
	}, "\\[SceneValidator\\] All domains must be connected\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the run command is called with a scene file
 * that contains overlapping domains.
 */
TEST(RunCommand, OverlappingDomains) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this filename that should trigger the error
	executeCommand("./OpenPSTD-cli create -f " + filename);
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -d [2,3,3,4] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -p [3,4] -f " + filename);
	
	// Execute the run command
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli run -f " + filename);
	}, "\\[SceneValidator\\] Domains cannot overlap\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the run command is called with sources that
 * are not inside any domain.
 */
TEST(RunCommand, SourceOutsideDomains) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this filename that should trigger the error
	executeCommand("./OpenPSTD-cli create -f " + filename);
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -p [0,3] -f " + filename);
	
	// Execute the run command
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli run -f " + filename);
	}, "\\[SceneValidator\\] All sources must be in a domain \\(0\\)\n");
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
	
	// Add a domain and a source to ensure that the scene is valid
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -d [4,2,2,4] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -d [0,6,2,2] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -p [2,3] -f " + filename);
	
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