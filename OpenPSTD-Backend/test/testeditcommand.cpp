#include <gtest/gtest.h>
#include "testcommand.h"
#include "../argumentparser.h"
#include "../editcommand.h"
#include "../settings.h"

constexpr double Settings::NEW_DOMAIN_ABSORPTION_TOP;
constexpr double Settings::NEW_DOMAIN_ABSORPTION_BOTTOM;
constexpr double Settings::NEW_DOMAIN_ABSORPTION_LEFT;
constexpr double Settings::NEW_DOMAIN_ABSORPTION_RIGHT;
const bool Settings::NEW_DOMAIN_ELR_TOP;
const bool Settings::NEW_DOMAIN_ELR_BOTTOM;
const bool Settings::NEW_DOMAIN_ELR_LEFT;
const bool Settings::NEW_DOMAIN_ELR_RIGHT;

/**
 * Verifies that the program exits with the correct error
 * message if the edit command is called without a filename
 * option.
 */
TEST(EditCommand, NoFilename) {
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit");
	}, "\\[EditCommand\\] Option \"-f\" is required\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the edit command is called without a filename
 * behind the filename option.
 */
TEST(EditCommand, EmptyFilename) {
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -f");
	}, "\\[EditCommand\\] Option \"-f\" requires a filename\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the value of the add domain option cannot be
 * parsed as a segment string.
 */
TEST(EditCommand, AddDomainInvalidSegmentString) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -d one -f " + filename);
	}, "\\[EditCommand\\] Parse error in coordinates\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the value of the add domain option contains
 * an incorrect number of segments.
 */
TEST(EditCommand, AddDomainIncorrectSegmentCount) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -d [1,2,3,4,5] -f " + filename);
	}, "\\[EditCommand\\] Parse error in location\n");
}

/**
 * Verifies that a domain is added at the correct coordinates
 * if the add domain option is used correctly.
 */
TEST(EditCommand, AddDomainCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to the scene file
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Assert that the scene file contains exactly one domain, with the correct coordinates
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.domains.size(), 1);
	ASSERT_EQ(conf.domains[0].left, 1);
	ASSERT_EQ(conf.domains[0].bottom, 2);
	ASSERT_EQ(conf.domains[0].width, 3);
	ASSERT_EQ(conf.domains[0].height, 4);
}

/**
 * Verifies that the program exits with the correct error
 * message if the domain ID to be removed does not exist.
 */
TEST(EditCommand, RemoveDomainNonExistingDomainID) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -D 0 -f " + filename);
	}, "\\[EditCommand\\] Domain id \"0\" does not exist\n");
}

/**
 * Verifies that the given domain is removed if the remove
 * domain option is used correctly.
 */
TEST(EditCommand, RemoveDomainCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Remove the domain with ID 0
	executeCommand("./OpenPSTD-cli edit -D 0 -f " + filename);
	
	// Assert that the scene file contains no more domains
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.domains.size(), 0);
}

/**
 * Verifies that the program exits with the correct error
 * message if the value of the set absorption option cannot
 * be parsed as a segment string.
 */
TEST(EditCommand, SetAbsorptionIncorrectSegmentString) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Set the domain absorption of the added domain
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -a one -f " + filename);
	}, "\\[EditCommand\\] Parse error in coordinates\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the value of the set absorption option contains
 * an incorrect number of segments.
 */
TEST(EditCommand, SetAbsorptionIncorrectSegmentCount) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Set the domain absorption of the added domain
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -a [1,2,3,4] -f " + filename);
	}, "\\[EditCommand\\] Parse error in absorption\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the domain ID of which to set the absorption
 * values does not exist.
 */
TEST(EditCommand, SetAbsorptionNonExistingDomainID) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Set the domain absorption of a non existing domain
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -a [0,t,0.5] -f " + filename);
	}, "\\[EditCommand\\] Domain id \"0\" does not exist\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the side of which to set the absorption
 * values is invalid.
 */
TEST(EditCommand, SetAbsorptionInvalidSide) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Set the domain absorption of an invalid side
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -a [0,one,0.5] -f " + filename);
	}, "\\[EditCommand\\] Parse error in absorption\n");
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -a [0,a,0.5] -f " + filename);
	}, "\\[EditCommand\\] Parse error in absorption\n");
}

/**
 * Verifies that the absorption values of the given domain
 * are updated correctly if the set absorption option is
 * used correctly.
 */
TEST(EditCommand, SetAbsorptionCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Set the domain absorption of the added domain
	executeCommand("./OpenPSTD-cli edit -a [0,t,0.5] -f " + filename);
	
	// Assert that the domain absorption has been altered correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.domains[0].absorptionTop, 0.5);
	ASSERT_EQ(conf.domains[0].absorptionBottom, Settings::NEW_DOMAIN_ABSORPTION_BOTTOM);
	ASSERT_EQ(conf.domains[0].absorptionLeft, Settings::NEW_DOMAIN_ABSORPTION_LEFT);
	ASSERT_EQ(conf.domains[0].absorptionRight, Settings::NEW_DOMAIN_ABSORPTION_RIGHT);
}

/**
 * Verifies that the program exits with the correct error
 * message if the value of the set elr option cannot be
 * parsed as a segment string.
 */
TEST(EditCommand, SetELRIncorrectSegmentString) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Set the elr values of the added domain
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -l one -f " + filename);
	}, "\\[EditCommand\\] Parse error in coordinates\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the value of the set elr option contains an
 * incorrect number of segments.
 */
TEST(EditCommand, SetELRIncorrectSegmentCount) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Set the elr values of the added domain
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -l [1,2,3,4] -f " + filename);
	}, "\\[EditCommand\\] Parse error in ELR\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the domain ID of which to set the elr values
 * does not exist.
 */
TEST(EditCommand, SetELRNonExistingDomainID) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Set the elr values of a non existing domain
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -l [0,t,0] -f " + filename);
	}, "\\[EditCommand\\] Domain id \"0\" does not exist\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the side of which to set the elr values is
 * invalid.
 */
TEST(EditCommand, SetELRInvalidSide) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Set the elr values of an invalid side
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -l [0,one,0] -f " + filename);
	}, "\\[EditCommand\\] Parse error in ELR\n");
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -l [0,a,0] -f " + filename);
	}, "\\[EditCommand\\] Parse error in ELR\n");
}

/**
 * Verifies that the program exits with the correct error
 * message if the elr value to be set is invalid.
 */
TEST(EditCommand, SetELRInvalidValue) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Set an invalid elr value
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -l [0,t,one] -f " + filename);
	}, "\\[EditCommand\\] Parse error in ELR\n");
}

/**
 * Verifies that the elr values of the given domain are
 * updated correctly if the set elr option is used
 * correctly.
 */
TEST(EditCommand, SetELRCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a domain to ensure that domain ID 0 exists
	executeCommand("./OpenPSTD-cli edit -d [1,2,3,4] -f " + filename);
	
	// Set the domain absorption of the added domain
	executeCommand("./OpenPSTD-cli edit -l [0,t,1] -f " + filename);
	
	// Assert that the domain absorption has been altered correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.domains[0].elrTop, true);
	ASSERT_EQ(conf.domains[0].elrBottom, Settings::NEW_DOMAIN_ELR_BOTTOM);
	ASSERT_EQ(conf.domains[0].elrLeft, Settings::NEW_DOMAIN_ELR_LEFT);
	ASSERT_EQ(conf.domains[0].elrRight, Settings::NEW_DOMAIN_ELR_RIGHT);
}

/**
 * Verifies that the program exits with the correct error
 * message if the value of the add source option contains
 * an incorrect number of segments.
 */
TEST(EditCommand, AddSourceIncorrectSegmentCount) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -p [1,2,3] -f " + filename);
	}, "\\[EditCommand\\] Parse error in location\n");
}

/**
 * Verifies that a source is added at the correct coordinates
 * if the add source option is used correctly.
 */
TEST(EditCommand, AddSourceCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a source to the scene file
	executeCommand("./OpenPSTD-cli edit -p [1,2] -f " + filename);
	
	// Assert that the scene file contains exactly one source, with the correct coordinates
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.sources.size(), 1);
	ASSERT_EQ(conf.sources[0].position_x, 1);
	ASSERT_EQ(conf.sources[0].position_y, 2);
}

/**
 * Verifies that the program exits with the correct error
 * message if the source ID to be removed does not exist.
 */
TEST(EditCommand, RemoveSourceNonExistingSourceID) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -P 0 -f " + filename);
	}, "\\[EditCommand\\] Source id \"0\" does not exist\n");
}

/**
 * Verifies that the given source is removed if the remove
 * source option is used correctly.
 */
TEST(EditCommand, RemoveSourceCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a source to ensure that source ID 0 exists
	executeCommand("./OpenPSTD-cli edit -p [1,2] -f " + filename);
	
	// Remove the source with ID 0
	executeCommand("./OpenPSTD-cli edit -P 0 -f " + filename);
	
	// Assert that the scene file contains no more sources
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.sources.size(), 0);
}

/**
 * Verifies that the program exits with the correct error
 * message if the value of the add receiver option contains
 * an incorrect number of segments.
 */
TEST(EditCommand, AddReceiverIncorrectSegmentCount) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -r [1,2,3] -f " + filename);
	}, "\\[EditCommand\\] Parse error in location\n");
}

/**
 * Verifies that a source is added at the correct coordinates
 * if the add receiver option is used correctly.
 */
TEST(EditCommand, AddReceiverCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a source to the scene file
	executeCommand("./OpenPSTD-cli edit -r [1,2] -f " + filename);
	
	// Assert that the scene file contains exactly one source, with the correct coordinates
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.receivers.size(), 1);
	ASSERT_EQ(conf.receivers[0].position_x, 1);
	ASSERT_EQ(conf.receivers[0].position_y, 2);
}

/**
 * Verifies that the program exits with the correct error
 * message if the receiver ID to be removed does not exist.
 */
TEST(EditCommand, RemoveReceiverNonExistingReceiverID) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Assert that the program exits with the correct error message
	ASSERT_DEATH({
		executeCommand("./OpenPSTD-cli edit -R 0 -f " + filename);
	}, "\\[EditCommand\\] Receiver id \"0\" does not exist\n");
}

/**
 * Verifies that the given receiver is removed if the remove
 * receiver option is used correctly.
 */
TEST(EditCommand, RemoveReceiverCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Add a receiver to ensure that receiver ID 0 exists
	executeCommand("./OpenPSTD-cli edit -r [1,2] -f " + filename);
	
	// Remove the receiver with ID 0
	executeCommand("./OpenPSTD-cli edit -R 0 -f " + filename);
	
	// Assert that the scene file contains no more receivers
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.receivers.size(), 0);
}

/**
 * Verifies that the grid spacing is updated correctly if the
 * set grid spacing option is used correctly.
 */
TEST(EditCommand, SetGridSpacingCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Set the grid spacing
	executeCommand("./OpenPSTD-cli edit --grid-spacing 0.5 -f " + filename);
	
	// Assert that the grid spacing is updated correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.gridSpacing, 0.5);
}

/**
 * Verifies that the window size is updated correctly if the
 * set window size option is used correctly.
 */
TEST(EditCommand, SetWindowSizeCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Set the window size
	executeCommand("./OpenPSTD-cli edit --window-size 16 -f " + filename);
	
	// Assert that the window size is updated correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.windowSize, 16);
}

/**
 * Verifies that the render time is updated correctly if the
 * set render time option is used correctly.
 */
TEST(EditCommand, SetRenderTimeCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Set the render time
	executeCommand("./OpenPSTD-cli edit --render-time 0.5 -f " + filename);
	
	// Assert that the render time is updated correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.renderTime, 0.5);
}

/**
 * Verifies that the air density is updated correctly if the
 * set air density option is used correctly.
 */
TEST(EditCommand, SetAirDensityCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Set the air density
	executeCommand("./OpenPSTD-cli edit --density-of-air 1.1 -f " + filename);
	
	// Assert that the air density is updated correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.airDensity, 1.1);
}

/**
 * Verifies that the sound speed is updated correctly if the
 * set sound speed option is used correctly.
 */
TEST(EditCommand, SetSoundSpeedCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Set the sound speed
	executeCommand("./OpenPSTD-cli edit --sound-speed 330 -f " + filename);
	
	// Assert that the sound speed is updated correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.soundSpeed, 330);
}

/**
 * Verifies that the number of pml cells is updated correctly
 * if the set pml cells option is used correctly.
 */
TEST(EditCommand, SetPMLCellsCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Set the number of pml cells
	executeCommand("./OpenPSTD-cli edit --pml-cells 40 -f " + filename);
	
	// Assert that the number of pml cells is updated correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.pmlCells, 40);
}

/**
 * Verifies that the attenuation of the pml cells is updated
 * correctly if the set attenuation option is used correctly.
 */
TEST(EditCommand, SetAttenuationCorrect) {
	// Generate a unique filename
	std::string filename = getUniqueFilename();
	
	// Create a scene file with this name
	executeCommand("./OpenPSTD-cli create -f " + filename);
	
	// Set the attenuation of the pml cells
	executeCommand("./OpenPSTD-cli edit --attenuation-of-pml-cells 10000 -f " + filename);
	
	// Assert that the attenuation of the pml cells is updated correctly
	Configuration conf = FileIO::readSceneFile(filename);
	ASSERT_EQ(conf.attenuation, 10000);
}