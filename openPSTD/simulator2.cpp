#include "simulator2.h"

Simulator2::Simulator2(QWidget* parent, QStatusBar* statusbar) {
	// Initialize the state variables
	this->threadRunning = false;
	this->statusbar = statusbar;
	
	this->so = new SimulatorOutput();
	
	parent->connect(this, SIGNAL(updateText(QString)), this, SLOT(setText(QString)));
	parent->connect(this, SIGNAL(centerScene()), parent, SLOT(slot_movetocenter()));
}

Simulator2::~Simulator2() {
	delete so;
}

void Simulator2::start() {
	// Do nothing if the simulator is already running
	Model* model = ModelManager::getInstance()->getCurrent();
	if (model->simulating) return;
	
	// Verify that the scene is valid
	if (!sceneValid()) return;
	
	// Update the status bar text
	model->simulating = true;
	emit updateText("Status: Starting Simulator");
	
	// Disable all interfering actions
	so->updateActions();
	
	// Show the simulator output
	so->setShown(true);
	
	// Center the scene
	emit centerScene();
	
	// Select the first receiver by default
	// TODO
	
	// Start a new thread for the Simulator
	threadRunning = true;
	threadID = pthread_create(&thread, nullptr, &Simulator2::run, this);
}

void Simulator2::stop() {
	// Do nothing if the simulator is not running
	Model* model = ModelManager::getInstance()->getCurrent();
	if (!model->simulating) return;
	
	// TODO: Request a stop through a boolean variable.
	// If pressed twice, force stop the thread.
	emit updateText("Status: Stopping Simulator");
	
	// Update the status bar text
	model->simulating = false;
	
	// Re-enable the state selecting actions
	so->updateActions();
}

void* Simulator2::run(void* args) {
	// Get the Simulator instance from the args parameter
	Simulator2* instance = static_cast<Simulator2*>(args);
	
	// Create a file for this simulation
	emit instance->updateText("Status: Initializing kernel");
	instance->exec(instance->kernel + " create " + instance->filename);
	
	// Delete all default domains, sources, and receivers
	instance->exec(instance->kernel + " edit -P 0 -f " + instance->filename);
	instance->exec(instance->kernel + " edit -R 0 -f " + instance->filename);
	instance->exec(instance->kernel + " edit -D 0 -f " + instance->filename);
	instance->exec(instance->kernel + " edit -D 0 -f " + instance->filename);
	
	// Add all domains
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Add the initial domain
		int x = model->domains[i]->getMinX();
		int y = model->domains[i]->getMinY();
		int w = model->domains[i]->getMaxX() - x;
		int h = model->domains[i]->getMaxY() - y;
		std::string cmd = "";
		cmd += instance->kernel + " edit -d [";
		cmd += std::to_string(x) + "," + std::to_string(y) + ",";
		cmd += std::to_string(w) + "," + std::to_string(h);
		cmd += "] -f " + instance->filename;
		instance->exec(cmd);
		
		// Set the absorption coefficients of the walls
		// TODO
		
		// Set the ELR values of the walls
		// TODO
	}
	
	// Add all sources
	for (unsigned int i = 0; i < model->sources.size(); i++) {
		// Add the source
		int x = model->sources[i]->getX();
		int y = model->sources[i]->getY();
		std::string cmd = "";
		cmd += instance->kernel + " edit -p [";
		cmd += std::to_string(x) + "," + std::to_string(y);
		cmd += "] -f " + instance->filename;
		instance->exec(cmd);
	}
	
	// Add all receivers
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		// Add the receiver
		int x = model->receivers[i]->getX();
		int y = model->receivers[i]->getY();
		std::string cmd = "";
		cmd += instance->kernel + " edit -r [";
		cmd += std::to_string(x) + "," + std::to_string(y);
		cmd += "] -f " + instance->filename;
		instance->exec(cmd);
	}
	
	// Update the PSTD settings
	// TODO (grid spacing, window size, render time, ...)
	
	// Run the simulation
	emit instance->updateText("Status: Running simulation");
	instance->runSimulation(instance->kernel + " run -f " + instance->filename);
	
	// Remove the scene file
	//instance->statusbar->showMessage("Status: Ready");
	remove(instance->filename.c_str());
	
	// Don't return anything
	return nullptr;
}

bool Simulator2::sceneValid() {
	// Update the status bar text
	emit updateText("Status: Validating scene");
	
	// Verify that there is at least one domain
	Model* model = ModelManager::getInstance()->getCurrent();
	if (model->domains.size() == 0) {
		emit updateText("Status: Error occurred (at least one domain is required)");
		return false;
	}
	
	// Verify that there is at least one source
	if (model->sources.size() == 0) {
		emit updateText("Status: Error occurred (at least one source is required)");
		return false;
	}
	
	// Verify that there is at least one receiver
	if (model->receivers.size() == 0) {
		emit updateText("Status: Error occurred (at least one receiver is required)");
		return false;
	}
	
	// Verify that all domains are connected
	if (!domainsConnected()) {
		emit updateText("Status: Error occurred (not all domains are connected)");
		return false;
	}
	
	// Verify that no domains overlap
	if (domainsOverlap()) {
		emit updateText("Status: Error occurred (domains should not overlap)");
		return false;
	}
	
	// Verify that all sources are inside a domain
	if (!sourcesInDomain()) {
		emit updateText("Status: Error occurred (all sources should be inside a domain)");
		return false;
	}
	
	// Verify that all receivers are inside a domain
	if (!receiversInDomain()) {
		emit updateText("Status: Error occurred (all receivers should be inside a domain)");
		return false;
	}
	
	// No errors occurred, the scene is valid
	return true;
}

bool Simulator2::domainsConnected() {
	// Initialize a set of all domain ids that are connected
	std::set<unsigned int> connected;
	connected.insert(0);
	
	// Keep track of the size of the connected set at the previous iteration
	unsigned long prevSize = 0;
	
	// Loop until prevSize does not change anymore
	Model* model = ModelManager::getInstance()->getCurrent();
	while (connected.size() != prevSize) {
		// Save the prevSize
		prevSize = connected.size();
		
		// Loop through all domains
		for (unsigned int i = 0; i < model->domains.size(); i++) {
			// Do nothing if this domain is already connected
			if (connected.find(i) != connected.end()) continue;
			
			// Check if this domain connects to any of the connected domains
			std::set<unsigned int>::iterator it;
			for (it = connected.begin(); it != connected.end(); it++) {
				int pl = model->domains[i]->getMinX();
				int pr = model->domains[i]->getMaxX();
				int pb = model->domains[i]->getMinY();
				int pt = model->domains[i]->getMaxY();
				int ql = model->domains[*it]->getMinX();
				int qr = model->domains[*it]->getMaxX();
				int qb = model->domains[*it]->getMinY();
				int qt = model->domains[*it]->getMaxY();
				if ((pl == qr || pr == ql) && (pt > qb && pb < qt)) {
					// The domains i and domainID are connected horizontally
					connected.insert(i);
					break;
				}
				if ((pb == qt || pt == qb) && (pr > ql && pl < qr)) {
					// The domains i and domainID are connected vertically
					connected.insert(i);
					break;
				}
			}
		}
	}
	
	// All domains are connected iff all domains are in the set connected
	return connected.size() == model->domains.size();
}

bool Simulator2::domainsOverlap() {
	// Loop through all pairs of domains
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		for (unsigned int j = i+1; j < model->domains.size(); j++) {
			// Check if domains i and j overlap
			int pl = model->domains[i]->getMinX();
			int pr = model->domains[i]->getMaxX();
			int pb = model->domains[i]->getMinY();
			int pt = model->domains[i]->getMaxY();
			int ql = model->domains[j]->getMinX();
			int qr = model->domains[j]->getMaxX();
			int qb = model->domains[j]->getMinY();
			int qt = model->domains[j]->getMaxY();
			bool overlapX = (pr > ql && pl < qr);
			bool overlapY = (pt > qb && pb < qt);
			if (overlapX && overlapY) return true;
		}
	}
	
	// No domains overlap
	return false;
}

bool Simulator2::sourcesInDomain() {
	// Loop through all sources
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->sources.size(); i++) {
		// Loop through all domains
		bool match = false;
		for (unsigned int j = 0; j < model->domains.size(); j++) {
			// Check if source i is in domain j
			int x = model->sources[i]->getX();
			int y = model->sources[i]->getY();
			int l = model->domains[j]->getMinX();
			int r = model->domains[j]->getMaxX();
			int b = model->domains[j]->getMinY();
			int t = model->domains[j]->getMaxY();
			bool xmatch = (l < x && x < r);
			bool ymatch = (b < y && y < t);
			if (xmatch && ymatch) match = true;
		}
		if (!match) return false;
	}
	
	// All sources are in a domain
	return true;
}

bool Simulator2::receiversInDomain() {
	// Loop through all receivers
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		// Loop through all domains
		bool match = false;
		for (unsigned int j = 0; j < model->domains.size(); j++) {
			// Check if receiver i is in domain j
			int x = model->receivers[i]->getX();
			int y = model->receivers[i]->getY();
			int l = model->domains[j]->getMinX();
			int r = model->domains[j]->getMaxX();
			int b = model->domains[j]->getMinY();
			int t = model->domains[j]->getMaxY();
			bool xmatch = (l < x && x < r);
			bool ymatch = (b < y && y < t);
			if (xmatch && ymatch) match = true;
		}
		if (!match) return false;
	}
	
	// All receivers are in a domain
	return true;
}

void Simulator2::exec(std::string cmd) {
	// Output the command being executed
	std::cout << "> \"" << cmd << "\"" << std::endl;
	
	// Use popen to execute the command
	// This is to be able to read its output
	FILE* fp = popen(cmd.c_str(), "r");
	char buffer[40];
	std::string line = "";
	while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
		// Add the received buffer to the line
		line += buffer;
		
		// Check if a newline character was received
		if (std::string(buffer).back() == '\n' && line != "\n") {
			// Print the received line
			line.pop_back(); // Remove the \n character
			std::cout << "< \"" << line << "\"" << std::endl;
			line = "";
		}
	}
	pclose(fp);
}

void Simulator2::runSimulation(std::string cmd) {
	// Output the command being executed
	std::cout << "> \"" << cmd << "\"" << std::endl;
	
	// Use popen to execute the command
	// This is to be able to read its output
	FILE* fp = popen(cmd.c_str(), "r");
	char buffer[40];
	std::string line = "";
	Model* model = ModelManager::getInstance()->getCurrent();
	while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
		// Add the received buffer to the line
		line += buffer;
		
		// Check if a newline character was received
		if (std::string(buffer).back() == '\n' && line != "\n") {
			// Print the received line
			line.pop_back(); // Remove the \n character
			std::cout << "< \"" << line << "\"" << std::endl;
			
			// Check if a new frame has been computed
			if (line.substr(0, frameFinishString.size()) == frameFinishString) {
				// Load the new frame
				std::string frameIDstring = line.substr(frameFinishString.size());
				int frameID = std::atoi(frameIDstring.c_str());
				so->loadFrame(frameID);
				
				// Emit an updateText signal, such that the
				// main thread will redraw the window
				emit updateText("Status: Running simulation");
			}
			
			// Check if the simulation has finished
			if (line == simulationFinishString) {
				// TODO
			}
			
			line = "";
		}
		
		// Check if the simulator should be stopped
		if (!model->simulating) {
			pclose(fp);
			emit updateText("Status: Ready");
			return;
		}
	}
	pclose(fp);
}