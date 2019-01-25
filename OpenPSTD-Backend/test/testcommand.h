#ifndef TESTCOMMAND_H
#define TESTCOMMAND_H

#include <gtest/gtest.h>
#include "../argumentparser.h"

/**
 * Handles operations provided by the test command.
 */
class TestCommand {
public:
	// Executes the test command.
	static void execute(ArgumentParser* parser);
};

#endif