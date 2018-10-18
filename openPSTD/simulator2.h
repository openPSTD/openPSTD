#ifndef SIMULATOR2_H
#define SIMULATOR2_H

#include <QStatusBar>
#include <pthread.h>
#include "modelmanager.h"
#include "frame.h"

class Simulator2 {
public:
	Simulator2(QStatusBar* statusbar);
	
	void start();
	void stop();
private:
	// Reference variables
	QStatusBar* statusbar;
	
	// Settings variables
	std::string kernel = "/home/jeroen/programming/openPSTD/GUI/build/OpenPSTD-cli";
	std::string filename = "test"; // Filename for the scene file (for kernel)
	std::string frameFinishString = "Finished frame: ";
	std::string simulationFinshString = "Succesfully finished simulation";
	
	// State variables
	int threadID;
	pthread_t thread;
	bool threadRunning;
	std::vector<Frame> frames;
	
	// Run simulator method
	static void* run(void* args);
	
	// Scene validation methods
	bool sceneValid();
	bool domainsConnected();
	bool domainsOverlap();
	bool sourcesInDomain();
	bool receiversInDomain();
	
	// Returns whether exactly one domain is selected
	bool domainSelected();
	
	void disableActions();
	void enableActions();
	
	void exec(std::string cmd);
	void runSimulation(std::string cmd);
};

#endif