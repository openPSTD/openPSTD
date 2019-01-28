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
	if (result != 0) exit(1);
}

std::string getUniqueFilename() {
	return std::tmpnam(nullptr);
}

void executeCommand(std::string command) {
	// Split the command on space
	std::vector<std::string> spl;
	spl.push_back("");
	for (unsigned int i = 0; i < command.size(); i++) {
		if (command[i] == ' ') {
			spl.push_back("");
		} else {
			spl.back() += command[i];
		}
	}
	
	// Setup argc, argv to match the given command
	int argc = static_cast<int>(spl.size());
	char* argv[argc];
	for (int i = 0; i < argc; i++) {
		argv[i] = &(spl[i])[0];
	}
	
	// Execute the command based on the second parameter
	ArgumentParser parser(argc, argv);
	if (spl[1] == "create") CreateCommand::execute(&parser);
	if (spl[1] == "list") ListCommand::execute(&parser);
	if (spl[1] == "edit") EditCommand::execute(&parser);
	if (spl[1] == "run") RunCommand::execute(&parser);
}