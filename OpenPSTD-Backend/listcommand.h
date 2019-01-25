#ifndef LISTCOMMAND_H
#define LISTCOMMAND_H

#include "argumentparser.h"
#include "configuration.h"
#include "fileio.h"

/**
 * Handles operations provided by the list command.
 */
class ListCommand {
public:
	// Executes the list command.
	static void execute(ArgumentParser* parser);
};

#endif