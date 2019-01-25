#ifndef EDITCOMMAND_H
#define EDITCOMMAND_H

#include <string>
#include <vector>
#include "argumentparser.h"
#include "configuration.h"
#include "fileio.h"
#include "settings.h"

/**
 * Handles operations provided by the edit command.
 */
class EditCommand {
public:
	// Executes the edit command.
	static void execute(ArgumentParser* parser);
private:
	// Private editing methods.
	static void addDomain(ArgumentParser* parser);
	static void removeDomain(ArgumentParser* parser);
	static void setAbsorption(ArgumentParser* parser);
	static void setELR(ArgumentParser* parser);
	static void addSource(ArgumentParser* parser);
	static void removeSource(ArgumentParser* parser);
	static void addReceiver(ArgumentParser* parser);
	static void removeReceiver(ArgumentParser* parser);
	static void setGridSpacing(ArgumentParser* parser);
	static void setWindowSize(ArgumentParser* parser);
	static void setRenderTime(ArgumentParser* parser);
	static void setAirDensity(ArgumentParser* parser);
	static void setSoundSpeed(ArgumentParser* parser);
	static void setPMLCells(ArgumentParser* parser);
	static void setAttenuation(ArgumentParser* parser);
	
	// Private string parsing methods.
	static std::vector<std::string> parseSegments(std::string s);
};

#endif