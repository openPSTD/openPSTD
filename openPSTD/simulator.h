#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <string>
#include <array>
#include <memory>
#include <thread>
#include <QImage>
#include <QAction>
#include "model.h"
#include <iostream>

class Simulator {
public:
    Simulator(Model* model, QAction* showoutput);
    ~Simulator();
    void start();
    void draw(QImage* pixels);
    void toggle();
private:
    Model* model;
    QAction* showoutput;
    bool shown;
    bool threadRunning;
    std::vector<std::string> output;
    std::thread thread;
    
    void run();
    void runcmd(std::string cmd);
    void drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels);
};

#endif