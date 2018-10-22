#ifndef SIMULATOROUTPUT_H
#define SIMULATOROUTPUT_H

#include <vector>
#include <mutex>
#include "frame.h"
#include "point.h"
#include "simulatorbutton.h"

class SimulatorOutput {
public:
	SimulatorOutput();
	
	void draw(QImage* pixels);
	inline void toggle() { this->shown = !this->shown; updateActions(); }
	inline void setShown(bool shown) { this->shown = shown; updateActions(); }
	unsigned int getOutputHeight() { return outputHeight; }
	
	void mousePress(int x, int y);
	
	void loadFrame(int frameID);
	
	void updateActions();
private:
	std::vector<Frame> frames;
	std::mutex framesMutex;
	std::vector<SimulatorButton> buttons;
	
	const unsigned int outputHeight = 200;
	QRgb backgroundColor = qRgb(100, 100, 100);
	
	bool shown;
	unsigned int shownFrame;
	
	void drawScene(QImage* pixels);
	void drawBackground(QImage* pixels);
	void drawPressureGraph(QImage* pixels);
	void drawButtons(QImage* pixels);
	
	QRgb sampleValue2Color(double value);
};

#endif