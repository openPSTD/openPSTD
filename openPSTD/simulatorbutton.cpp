#include "simulatorbutton.h"

SimulatorButton::SimulatorButton(
    ButtonType type,
    int* shownFrame,
    int* frameSpeed,
    std::vector<Frame>* frames
) {
	// Initialize representation variables
	this->type = type;
    
    // Initialize reference variables
    this->shownFrame = shownFrame;
    this->frameSpeed = frameSpeed;
    this->frames = frames;
    
    // Initialize state variables
	this->initialized = false;
}

void SimulatorButton::draw(int x, int y, int s, QImage* pixels) {
	// Update the state variables
	this->x = x;
	this->y = y;
	this->s = s;
	this->initialized = true;
	
	// Draw button background
	drawBackground(x, y, s, pixels);
}

void SimulatorButton::mousePress(int x, int y) {
	// Do nothing if the state variables have not been initialized yet
	if (!initialized) return;
	
    // Do nothing if this button was not pressed
    if (std::abs(this->x - x) > this->s/2 || std::abs(this->y - y) > this->s/2) {
        return;
    }
    
    // Update the shownFrame variable based on this button's type
    if (type == BEGIN) *shownFrame = 0;
    if (type == BACK)  *frameSpeed = -1;
    if (type == PAUSE) *frameSpeed = 0;
    if (type == PLAY)  *frameSpeed = 1;
    if (type == END)   *shownFrame = frames->size() - 1;
}

void SimulatorButton::drawBackground(int x, int y, int s, QImage* pixels) {
	std::string filename = "";
	if (type == BEGIN) filename = ":/new/prefix1/icons/output_home.png";
	if (type == BACK)  filename = ":/new/prefix1/icons/output_playreversed.png";
	if (type == PAUSE) filename = ":/new/prefix1/icons/output_pause.png";
	if (type == PLAY)  filename = ":/new/prefix1/icons/output_play.png";
	if (type == END)   filename = ":/new/prefix1/icons/output_end.png";
	drawImage(x-s/2, y-s/2, s, filename, pixels);
}

void SimulatorButton::drawImage(int x, int y, int s, std::string filename, QImage* pixels) {
	QPainter p;
	p.begin(pixels);
	p.drawImage(QRect(x+1, y+1, s-2, s-2), QImage(filename.c_str()));
	p.drawRect(x, y, s-1, s-1);
	p.end();
}