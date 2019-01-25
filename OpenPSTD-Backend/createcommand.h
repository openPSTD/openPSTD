#ifndef CREATECOMMAND_H
#define CREATECOMMAND_H

#include "argumentparser.h"
#include "configuration.h"
#include "fileio.h"
#include "settings.h"

/**
 * Handles operations provided by the create command.
 */
class CreateCommand {
public:
	// Executes the create command.
	static void execute(ArgumentParser* parser);
private:
	// Creates a default Configuration instance.
	static Configuration getDefaultConfiguration();
};

#endif