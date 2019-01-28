#include <string>
#include <gtest/gtest.h>
#include "testcommand.h"
#include "../argumentparser.h"
#include "../listcommand.h"

/**
 * Verifies that the program exits with the correct error
 * message if the list command is called without a filename
 * option.
 */
TEST(ListCommand, NoFilename) {
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli list");
	}, "\\[ListCommand\\] Option \"-f\" is required\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the list command is called without a filename
 * behind the filename option.
 */
TEST(ListCommand, EmptyFilename) {
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli list -f");
	}, "\\[ListCommand\\] Option \"-f\" requires a filename\n");
}

/**
 * Verifies that the correct output is printed to stdout if
 * the list command is used correctly.
 */
TEST(ListCommand, Output) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name, and add a domain, source, and receiver
	executeCommand("./OpenPSTD-cli create -f " + filename);
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -p [5,6] -f " + filename);
	executeCommand("./OpenPSTD-cli edit -r [7,8] -f " + filename);
	
	// Execute the list command
	testing::internal::CaptureStdout();
	executeCommand("./OpenPSTD-cli list -f " + filename);
	std::string output = testing::internal::GetCapturedStdout();
	
	// Assert that one domain was printed to stdout
	ASSERT_NE(output.find("domain 0"), std::string::npos);
	ASSERT_EQ(output.find("domain 1"), std::string::npos);
	
	// Assert that one source was printed to stdout
	ASSERT_NE(output.find("source 0"), std::string::npos);
	ASSERT_EQ(output.find("source 1"), std::string::npos);
	
	// Assert that one receiver was printed to stdout
	ASSERT_NE(output.find("receiver 0"), std::string::npos);
	ASSERT_EQ(output.find("receiver 1"), std::string::npos);
	
	// Assert that all settings were printed to stdout
	ASSERT_NE(output.find("gridSpacing: "), std::string::npos);
	ASSERT_NE(output.find("\nwindowSize: "), std::string::npos);
	ASSERT_NE(output.find("\nrenderTime: "), std::string::npos);
	ASSERT_NE(output.find("\nairDensity: "), std::string::npos);
	ASSERT_NE(output.find("\nsoundSpeed: "), std::string::npos);
	ASSERT_NE(output.find("\npmlCells: "), std::string::npos);
	ASSERT_NE(output.find("\nattenuation: "), std::string::npos);
}