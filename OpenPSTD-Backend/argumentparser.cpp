#include "argumentparser.h"

/**
 * Constructor.
 * Initializes the representation variables.
 * 
 * @param argc  The number of arguments in argv
 * @param argv  An array of all supplied arguments
 */
ArgumentParser::ArgumentParser(int argc, char** argv) {
	// Initialize the representation variables
	this->argc = argc;
	this->argv = argv;
	
	// First argument is the command
	if (argc < 2) {
		std::cerr << "Usage: ./OpenPSTD-cli <command> [options]" << std::endl;
		exit(1);
	}
	std::string cmd = argv[1];
	if (cmd == "create") command = CREATE;
	else if (cmd == "list") command = LIST;
	else if (cmd == "edit") command = EDIT;
	else if (cmd == "run") command = RUN;
	else if (cmd == "test") command = TEST;
	else {
		std::cerr << "[ArgumentParser] Invalid command \"" << cmd << "\"" << std::endl;
		exit(1);
	}
	
	// Following arguments are the options
	for (int i = 2; i < argc; i++) {
		// Verify that this option is prefixed with a dash
		std::string option = argv[i];
		if (option[0] != '-') {
			std::cerr << "[ArgumentParser] Options must be prefixed ";
			std::cerr << "with a dash (word " << i << ")" << std::endl;
			exit(1);
		}
		
		// Check if the next word is a value for this option
		std::string value = "";
		if (i+1 < argc && argv[i+1][0] != '-') value = argv[++i];
		
		// Save this option in the options map
		options[option] = value;
	}
	
	// Print the parsed options
	if (options.find("--debug") != options.end()) {
		std::cout << "ArgumentParser:" << std::endl;
		std::cout << "  Command: \"" << command << "\"";
		if (command == CREATE) std::cout << " (create)" << std::endl;
		if (command == LIST) std::cout << " (list)" << std::endl;
		if (command == EDIT) std::cout << " (edit)" << std::endl;
		if (command == RUN) std::cout << " (run)" << std::endl;
		if (command == TEST) std::cout << " (test)" << std::endl;
		for (auto it = options.begin(); it != options.end(); it++) {
			std::cout << "  Option: \"" << it->first << "\" -> \"";
			std::cout << it->second << "\"" << std::endl;
		}
	}
}

/**
 * Checks if a given option has been supplied, and
 * returns its value in the output parameter if available.
 * 
 * @param option  The option to check for (including the leading dash)
 * @param value  Output parameter for the value of the requested option
 * @return  Whether or not the given option has been supplied
 */
bool ArgumentParser::hasOption(std::string option, std::string* value) {
	// Verify that the option has been supplied
	if (options.find(option) == options.end()) return false;
	
	// Store the value of the requested option, and return
	if (value != nullptr) *value = options[option];
	return true;
}