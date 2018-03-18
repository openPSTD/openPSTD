#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <string>
#include <vector>
#include <array>
#include <memory>
#if defined(_WIN32) || defined(_WIN64)
#include "mingw.thread.h"
#else
#include <thread>
#endif
#include <QImage>
#include <QAction>
#include "model.h"
#include "frame.h"
#include <iostream>

class Simulator {
public:
    Simulator(Model* model, QAction* showoutput);
    ~Simulator();
    void start();
    void draw(QImage* pixels);
    void toggle();
    bool isShown();
    void showFrame(int x);
    void pressButton(int x);
private:
    Model* model;
    QAction* showoutput;
    bool shown;
    bool threadRunning;
    std::vector<std::string> output;
    std::thread thread;
    int numframes;
    int numcomputed;
    int shownFrame;
    int width;
    int playspeed;
    std::vector<Frame*> frames;
    
    void run();
    void runcmd(std::string cmd);
    void drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels);
    void drawImage(int x, int y, std::string filename, QImage* pixels);
    
    void button_home();
    void button_playreversed();
    void button_pause();
    void button_play();
    void button_end();
    void button_sound();
};

#endif