#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class Frame {
public:
    Frame(std::string filename);
    double getSample(int x, int y);
private:
    std::vector<double> samples;
    int width;
};

#endif