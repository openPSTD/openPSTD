#include <iostream>
#include "argumentparser.h"
#include "createcommand.h"
#include "listcommand.h"
#include "editcommand.h"
#include "runcommand.h"

int main(int argc, char** argv) {
	// Create an ArgumentParser instance from the supplied arguments
	ArgumentParser parser(argc, argv);
	
	// Delegate based on the command
	Command command = parser.getCommand();
	if (command == CREATE) CreateCommand::execute(&parser);
	if (command == LIST) ListCommand::execute(&parser);
	if (command == EDIT) EditCommand::execute(&parser);
	if (command == RUN) RunCommand::execute(&parser);
	
	// No error occurred
	return 0;
}