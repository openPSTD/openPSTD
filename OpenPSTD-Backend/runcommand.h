#ifndef RUNCOMMAND_H
#define RUNCOMMAND_H

#include "argumentparser.h"
#include "configuration.h"
#include "fileio.h"
#include "solver.h"

/**
 * Handles operations provided by the run command.
 */
class RunCommand {
public:
	// Executes the run command.
	static void execute(ArgumentParser* parser);
private:
	static void saveFrame(ArgumentParser* parser, Configuration* conf);
};

#endif