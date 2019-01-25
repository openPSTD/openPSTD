#include "testcommand.h"

/**
 * Executes the test command.
 * 
 * @param parser  A reference to an ArgumentParser instance
 *                containing the supplied arguments
 */
void TestCommand::execute(ArgumentParser* parser) {
	// Initialize GTest
	int argc = parser->getArgc();
	char** argv = parser->getArgv();
	testing::InitGoogleTest(&argc, argv);
	
	// Run all tests
	int result = RUN_ALL_TESTS();
	if (result != 0) {
		// Errors occurred
		exit(1);
	}
}