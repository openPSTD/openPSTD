#ifndef RECEIVER_H
#define RECEIVER_H

#include <QImage>
#include "settings.h"

/**
 * Representation of a single receiver.
 */
class Receiver {
public:
    // Constructor
    Receiver(int x, int y, Settings* settings);
    
    // Drawing method
    void draw(QImage* pixels, int zoom);
    
    // Get methods for the position of the receiver
    int getX() { return x; }
    int getY() { return y; }
private:
    // Class instance variables
    Settings* settings;
    
    // Position of the receiver
    int x;
    int y;
};

#endif