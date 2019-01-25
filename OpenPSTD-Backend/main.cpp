/**
 * TODO:
 * - FileIO::saveFrameFile
 * - Solver::initializePMLDomains
 * - Solver::computeDeltaP
 * - Solver::computeLEE
 * - Solver::applyWindowing
 * 
 * Testing:
 * - Create methods in FileIO for reading a vector of strings from a file,
 *   and for writing a vector of strings to a file. Then the rest of the
 *   FileIO class (parsing from/to a Configuration instance) can be tested.
 */
#include <iostream>
#include "argumentparser.h"
#include "createcommand.h"
#include "listcommand.h"
#include "editcommand.h"
#include "runcommand.h"
#include "test/testcommand.h"

int main(int argc, char** argv) {
	// Create an ArgumentParser instance from the supplied arguments
	ArgumentParser parser(argc, argv);
	
	// Delegate based on the command
	Command command = parser.getCommand();
	if (command == CREATE) CreateCommand::execute(&parser);
	if (command == LIST) ListCommand::execute(&parser);
	if (command == EDIT) EditCommand::execute(&parser);
	if (command == RUN) RunCommand::execute(&parser);
	if (command == TEST) TestCommand::execute(&parser);
	
	// No error occurred
	return 0;
}