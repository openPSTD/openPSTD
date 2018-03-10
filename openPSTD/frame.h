#ifndef FRAME_H
#define FRAME_H

class Frame {
public:
    Frame(int width, int height);
    ~Frame();
    void setSample(int x, int y, double value);
    double getSample(int x, int y);
private:
    double* samples;
    int width;
    int height;
    
    int index(int x,int y);
};

#endif