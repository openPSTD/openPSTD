#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <string>
#include <vector>
#include <unistd.h>
#include <array>
#include <memory>
#if defined(_WIN32) || defined(_WIN64)
#include "mingw.thread.h"
#else
#include <thread>
#endif
#include <mutex>
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
    void stop() {}
    
    void draw(QImage* pixels);
    void toggle();
    inline bool isShown() { return shown; }
    void showFrame(int x);
    void pressButton(int x);
    inline std::vector<Frame> getFrames() { 
        mutex.lock();
        std::vector<Frame> fs = frames;
        mutex.unlock();
        return fs;
    }
    void setReceiver(int i);
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
    int receiverID;
    int receiverDID;
    std::vector<Frame> frames;
    const std::string kernel = "../OpenPSTD-cli";
    std::mutex mutex;
    
    int brightness = 10;
    int scale = 200;
    
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