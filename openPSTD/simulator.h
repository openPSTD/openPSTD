#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <string>
#include <vector>
#include <unistd.h>
#include <array>
#include <memory>
#include <mutex>
#include <QImage>
#include <QAction>
#include <QStatusBar>
#include "model.h"
#include "frame.h"
#include <iostream>

class Simulator {
public:
    Simulator(Model* model, QAction* showoutput, QStatusBar* statusbar);
    ~Simulator();
    
    void start();
    void stop() {
        statusbar->showMessage("Status: Stopping simulation");
        pthread_cancel(thread);
        statusbar->showMessage("Status: Ready");
    }
    
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
    
    inline bool getCompleted() { return completed; }
private:
    Model* model;
    QAction* showoutput;
    QStatusBar* statusbar;
    bool shown;
    bool threadRunning;
    std::vector<std::string> output;
    pthread_t thread;
    int threadID;
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
    bool completed;
    
    int brightness = 10;
    int scale = 200;
    
    inline static void* runstatic(void* args) { ((Simulator*) args)->run(); }
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