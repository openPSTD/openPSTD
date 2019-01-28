#ifndef TESTCOMMAND_H
#define TESTCOMMAND_H

#include <string>
#include <gtest/gtest.h>
#include "../argumentparser.h"
#include "../createcommand.h"
#include "../listcommand.h"
#include "../editcommand.h"
#include "../runcommand.h"

/**
 * Handles operations provided by the test command.
 */
class TestCommand : public ::testing::Test {
public:
	// Executes the test command.
	static void execute(ArgumentParser* parser);
};

std::string getUniqueFilename();
void executeCommand(std::string command);

#endif