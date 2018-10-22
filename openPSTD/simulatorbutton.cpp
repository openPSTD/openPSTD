#include "simulatorbutton.h"

SimulatorButton::SimulatorButton(ButtonType type) {
	// Initialize representation variables
	this->type = type;
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
	
	// Check if this button was pressed
	if (std::abs(this->x - x) <= this->s/2 && std::abs(this->y - y) <= this->s/2) {
		std::cout << "BUTTON PRESSED" << std::endl;
	}
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