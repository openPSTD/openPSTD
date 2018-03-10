#include "frame.h"

Frame::Frame(int width, int height) {
    // Save the width and height
    this->width = width;
    this->height = height;
    
    // Initialize the samples array
    samples = new double[width * height];
}

Frame::~Frame() {
    // Delete the samples array
    delete samples;
}

void Frame::setSample(int x, int y, double value) {
    // Save the value of the sample
    samples[index(x, y)] = value;
}

double Frame::getSample(int x, int y) {
    // Verify that the sample exists
    int i = index(x, y);
    if (i < 0 || i > width) return 0;
    
    // Return the value of the sample
    return samples[i];
}

int Frame::index(int x, int y) {
    // Return the index in the samples array
    return x + y * width;
}
