#include "simulatoroutput.h"

SimulatorOutput::SimulatorOutput() {
	// Initialize state variables
	this->shown = false;
	this->shownFrame = 0;
    this->frameSpeed = 1;
	
	// Register the simulator buttons
	buttons.push_back(SimulatorButton(BEGIN, &shownFrame, &frameSpeed, &frames));
	buttons.push_back(SimulatorButton(BACK,  &shownFrame, &frameSpeed, &frames));
	buttons.push_back(SimulatorButton(PAUSE, &shownFrame, &frameSpeed, &frames));
	buttons.push_back(SimulatorButton(PLAY,  &shownFrame, &frameSpeed, &frames));
	buttons.push_back(SimulatorButton(END,   &shownFrame, &frameSpeed, &frames));
}

void SimulatorOutput::draw(QImage* pixels) {
	// Do nothing if the output should not be shown
	if (!shown) return;
    this->width = pixels->width();
    this->height = pixels->height();
	
    // Update shownFrame based on frameSpeed
    shownFrame += frameSpeed;
    if (shownFrame < 0) shownFrame = 0;
    if (shownFrame >= frames.size()) shownFrame = frames.size() - 1;
    
	// Draw the pressure overlay on the scene
	drawScene(pixels);
	
	// Draw the background
	drawBackground(pixels);
	
	// Draw the pressure graph
	drawPressureGraph(pixels);
	
	// Draw the buttons
	drawButtons(pixels);
}

void SimulatorOutput::mousePress(int x, int y) {
	// Delegate to the simulator buttons
    bool handled = false;
	for (unsigned int i = 0; i < buttons.size(); i++) {
		handled |= buttons[i].mousePress(x, y);
	}
    
    Model* model = ModelManager::getInstance()->getCurrent();
    if (!handled && y > height - model->simulatorHeight) {
        // Select the clicked frame
        int numFrames = model->rendertime * 340 / (0.5 * model->gridspacing);
        int frameID = numFrames * x / width;
        this->shownFrame = frameID;
    }
}

void SimulatorOutput::mouseDrag(int x, int y) {
    Model* model = ModelManager::getInstance()->getCurrent();
    if (y > height - model->simulatorHeight) {
        // Select the clicked frame
        int numFrames = model->rendertime * 340 / (0.5 * model->gridspacing);
        int frameID = numFrames * x / width;
        this->shownFrame = frameID;
    }
}

void SimulatorOutput::loadFrame(int frameID) {
	framesMutex.lock();
	frames.push_back(Frame(frameID));
	framesMutex.unlock();
}

void SimulatorOutput::updateActions() {
	// Update the simulator height
	Model* model = ModelManager::getInstance()->getCurrent();
	model->simulatorHeight = (shown ? outputHeight : 0);
	
	// Count the number of selected domains
	unsigned int numSelected = 0;
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		if (model->domains[i]->getSelected()) numSelected++;
	}
	
	// Enable / disable the state selecting actions
	model->actionSelect_Domain->setEnabled(!model->simulating);
	model->actionSelect->setEnabled(!model->simulating);
	model->actionMoveScene->setEnabled(!model->simulating);
	model->actionAddDomain->setEnabled(!model->simulating);
	model->actionAddSource->setEnabled(!model->simulating);
	model->actionAddReceiver->setEnabled(!model->simulating);
	model->actionMeasure->setEnabled(!model->simulating);
	model->actionUndo->setEnabled(!model->simulating && ModelManager::getInstance()->canUndo());
	model->actionRedo->setEnabled(!model->simulating && ModelManager::getInstance()->canRedo());
	model->actionMoveToCenter->setEnabled(!model->simulating);
	model->actionChangeAbsorption->setEnabled(!model->simulating && numSelected == 1);
	model->actionShowSidebar->setEnabled(!model->simulating);
	
	// Update the state of the show output action
	model->actionShow_Output->setChecked(shown);
    
	// Enable / disable the Stop Simulator action
	model->actionStop->setEnabled(model->simulating);
	model->actionRemoveSimulation->setEnabled(model->hasSimulationOutput);
	
    // Select the SELECT_DOMAIN action
    model->actionSelect_Domain->setChecked(true);
    model->state = SELECTDOMAIN;
}

void SimulatorOutput::reset() {
    // Reset the state variables
    this->frames.clear();
    this->shown = false;
    this->shownFrame = 0;
    this->frameSpeed = 1;
}

void SimulatorOutput::drawScene(QImage* pixels) {
	// Do nothing if the shown frame does not exist
	framesMutex.lock();
	if (shownFrame >= frames.size()) {
		framesMutex.unlock();
		return;
	}
	
	// Loop through all domains
	Model* model = ModelManager::getInstance()->getCurrent();
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Compute the dimensions of this domain
		int x0 = model->domains[i]->getMinXP()->getScreen().x();
		int x1 = model->domains[i]->getMaxXP()->getScreen().x();
		int y0 = model->domains[i]->getMaxYP()->getScreen().y();
		int y1 = model->domains[i]->getMinYP()->getScreen().y();
		
		// Get the width and height of the samples matrix
		unsigned int w = frames[shownFrame].getWidth(i);
		unsigned int h = frames[shownFrame].getHeight(i);
		
		// Loop through all samples
		for (int y = y0; y <= y1; y++) {
			for (int x = x0; x <= x1; x++) {
				// Compute the sample index for this pixel
				int sx = ((x-x0)*static_cast<int>(w)/(x1-x0));
				int sy = ((y-y0)*static_cast<int>(h)/(y1-y0));
                if (FLIP_Y_COORDINATES) {
                    sy = ((y1-y)*static_cast<int>(h)/(y1-y0));
                }
				
				// Get the sample value at this index
				double value = frames[shownFrame].getSample(
					static_cast<unsigned int>(sx),
					static_cast<unsigned int>(sy),
					i
				);
				
				// Convert this value into a color
				QRgb color = sampleValue2Color(value);
				
				// Draw this pixel
                if (x-1 < 0 || x-1 >= pixels->width()) continue;
                if (y-1 < 0 || y-1 >= pixels->height()) continue;
				pixels->setPixel(x-1, y-1, color);
			}
		}
	}
	framesMutex.unlock();
}

void SimulatorOutput::drawBackground(QImage* pixels) {
	// Draw the background
	for (int i = 0; i < pixels->width(); i++) {
		for (int j = pixels->height()-static_cast<int>(outputHeight); j < pixels->height(); j++) {
			pixels->setPixel(i, j, backgroundColor);
		}
	}
	
	// Draw a horizontal line for y=0
	int j = pixels->height() - static_cast<int>(outputHeight)/2;
	for (int i = 0; i < pixels->width(); i++) {
		pixels->setPixel(i, j, qRgb(0, 0, 0));
	}
}

void SimulatorOutput::drawPressureGraph(QImage* pixels) {
	// Do nothing if there are no frames yet
	if (frames.size() == 0) return;
	
	// Verify that exactly one receiver is selected
	Model* model = ModelManager::getInstance()->getCurrent();
	unsigned int numSelected = 0;
	unsigned int selectedID = 0;
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		if (model->receivers[i]->getSelected()) {
			numSelected++;
			selectedID = i;
		}
	}
	
	// Select receiver 0 if not exactly one receiver is selected
	if (numSelected != 1) {
		for (unsigned int i = 0; i < model->receivers.size(); i++) {
			model->receivers[i]->setSelected(i == 0);
		}
		selectedID = 0;
	}
	
	// Get the position of the selected receiver
	int x = model->receivers[selectedID]->getX();
	int y = model->receivers[selectedID]->getY();
	
	// Check which domain the selected receiver is in
	unsigned int domainID = 0;
	int sx, sy;
	for (unsigned int i = 0; i < model->domains.size(); i++) {
		// Get the coordinates of this domain
		int x0 = model->domains[i]->getMinX();
		int x1 = model->domains[i]->getMaxX();
		int y0 = model->domains[i]->getMinY();
		int y1 = model->domains[i]->getMaxY();
		
		// Check if the receiver is in this domain
		bool xmatch = x0 <= x && x <= x1;
		bool ymatch = y0 <= y && y <= y1;
		if (xmatch && ymatch) {
			// Save the domainID
			domainID = i;
			
			// Compute the sample coordinates
			unsigned int w = frames[0].getWidth(domainID);
			unsigned int h = frames[0].getHeight(domainID);
			sx = ((x-x0)*static_cast<int>(w)/(x1-x0));
			sy = ((y-y0)*static_cast<int>(h)/(y1-y0));
		}
	}
	
	// Loop through all frames
	std::vector<double> pressure;
    double maxSample;
	for (unsigned int frameID = 0; frameID < frames.size(); frameID++) {
		// Get the pressure value at the receiver position
		double p = frames[frameID].getSample(sx, sy, domainID);
		pressure.push_back(p);
        
        // Update the maxSample value
        if (frameID == 0 || std::abs(p) > maxSample) maxSample = std::abs(p);
	}
    
	// Loop through all x-coordinates of the pressure graph
    int numFrames = model->rendertime * 340 / (0.5 * model->gridspacing);
    for (unsigned int i = 0; i < frames.size()-1; i++) {
        double sample0 = pressure[i];
        int x0 = i * pixels->width() / numFrames;
        int y0 = outputHeight/2 + (sample0 * (outputHeight-34) / (2 * maxSample));
        
        double sample1 = pressure[i+1];
        int x1 = (i+1) * pixels->width() / numFrames;
        int y1 = outputHeight/2 + (sample1 * (outputHeight-34) / (2 * maxSample));
        
        if (std::abs(x1-x0) >= std::abs(y1-y0)) {
            for (int x = x0; x < x1; x++) {
                int y = y0 + (y1-y0)*(x-x0)/(x1-x0);
                pixels->setPixel(x, pixels->height()-y, qRgb(0, 255, 0));
            }
        } else {
            if (y1 < y0) {
                int xx = x1; x1 = x0; x0 = xx;
                int yy = y1; y1 = y0; y0 = yy;
            }
            for (int y = y0; y < y1; y++) {
                int x = x0 + (x1-x0)*(y-y0)/(y1-y0);
                pixels->setPixel(x, pixels->height()-y, qRgb(0, 255, 0));
            }
        }
    }
	
	// Draw a vertical line at the current frame
	int frameX = shownFrame * pixels->width() / numFrames;
	for (unsigned int y = pixels->height() - outputHeight; y < pixels->height(); y++) {
		pixels->setPixel(frameX, y, qRgb(255, 0, 0));
	}
}

void SimulatorOutput::drawButtons(QImage* pixels) {
	// Loop through all buttons
	int y = pixels->height() - static_cast<int>(outputHeight);
	int s = 34;
	int n = static_cast<int>(buttons.size());
	for (unsigned int i = 0; i < buttons.size(); i++) {
		// Compute the x-coordinate of this button
		int x = pixels->width()/2 - (n-1)*s/2 + static_cast<int>(i)*s;
		
		// Draw this button
		buttons[i].draw(x, y, s, pixels);
	}
}

QRgb SimulatorOutput::sampleValue2Color(double value) {
	double c = std::sqrt(value) * 255.0 * 2;
	return qRgb(static_cast<int>(c), 0, 0);
}