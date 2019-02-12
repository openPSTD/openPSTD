#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <string>
#include <map>
#include <iostream>

enum Command {
	CREATE,
	LIST,
	EDIT,
	RUN,
	TEST
};

/**
 * Parses the supplied command line options.
 */
class ArgumentParser {
public:
	// Constructor.
	ArgumentParser(int argc, char** argv);
	
	// Get methods for representation variables.
	inline Command getCommand() { return command; }
	bool hasOption(std::string option, std::string* value = nullptr);
	int getArgc() { return argc; }
	char** getArgv() { return argv; }
private:
	// Representation variables.
	Command command;
	std::map<std::string, std::string> options;
	int argc;
	char** argv;
};

#endif