#include "simulatoroutput.h"

SimulatorOutput::SimulatorOutput() {
	// Initialize state variables
	this->shown = false;
	this->shownFrame = 0;
	
	// Register the simulator buttons
	buttons.push_back(SimulatorButton(BEGIN));
	buttons.push_back(SimulatorButton(BACK));
	buttons.push_back(SimulatorButton(PAUSE));
	buttons.push_back(SimulatorButton(PLAY));
	buttons.push_back(SimulatorButton(END));
}

void SimulatorOutput::draw(QImage* pixels) {
	// Do nothing if the output should not be shown
	if (!shown) return;
	
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
	for (unsigned int i = 0; i < buttons.size(); i++) {
		buttons[i].mousePress(x, y);
	}
}

void SimulatorOutput::loadFrame(int frameID) {
	framesMutex.lock();
	frames.push_back(Frame(frameID));
	shownFrame = static_cast<unsigned int>(frameID);
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
				
				// Get the sample value at this index
				double value = frames[shownFrame].getSample(
					static_cast<unsigned int>(sx),
					static_cast<unsigned int>(sy),
					i
				);
				
				// Convert this value into a color
				QRgb color = sampleValue2Color(value);
				
				// Draw this pixel
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
	for (unsigned int frameID = 0; frameID < frames.size(); frameID++) {
		// Get the pressure value at the receiver position
		double p = frames[frameID].getSample(sx, sy, domainID);
		pressure.push_back(p);
	}
	
	// Compute the maximum sample value
	double maxSample = 1.0;
	
	// Loop through all x-coordinates of the pressure graph
	int numFrames = 340; // TODO
	for (int x = 0; x < pixels->width(); x++) {
		// Compute the index in the pressure graph of this x-coordinate
		double index = static_cast<double>(x) * numFrames / pixels->width();
		unsigned int il = static_cast<unsigned int>(index);
		unsigned int ih = static_cast<unsigned int>(index) + 1;
		
		// Do nothing if this index does not exist yet
		if (ih >= pressure.size()) break;
		
		// Interpolate the two surrounding samples
		double t = index - static_cast<int>(index);
		double sample = (1-t)*pressure[il] + t*pressure[ih];
		
		// Draw this sample
		int yy = static_cast<int>(sample * outputHeight / (2 * maxSample));
		int y = pixels->height() - static_cast<int>(outputHeight/2) - yy;
		pixels->setPixel(x, y, qRgb(0, 255, 0));
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