#ifndef SIMULATORBUTTON_H
#define SIMULATORBUTTON_H

#include <QImage>
#include <QPainter>
#include <iostream>
#include "frame.h"

enum ButtonType {
	BEGIN, // Go to frame 0
	BACK,  // Play backwards
	PAUSE, // Stop playing
	PLAY,  // Play
	END    // Go to the last computed frame
};

class SimulatorButton {
public:
	SimulatorButton(
        ButtonType type,
        int* shownFrame,
        int* frameSpeed,
        std::vector<Frame>* frames
    );
	void draw(int x, int y, int size, QImage* pixels);
	
	void mousePress(int x, int y);
private:
	// Representation variables
	ButtonType type;
    
    // Reference variables
    int* shownFrame;
    int* frameSpeed;
    std::vector<Frame>* frames;
	
	// State variables
	bool initialized;
	int x;
	int y;
	int s;
	
	void drawBackground(int x, int y, int size, QImage* pixels);
	
	void drawImage(int x, int y, int s, std::string filename, QImage* pixels);
};

#endif