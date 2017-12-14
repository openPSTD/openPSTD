#ifndef RECEIVER_H
#define RECEIVER_H

#include <stdexcept>
#include <QPoint>
#include <QImage>
#include "utility.h"
#include "settings.h"
#include "modelmanager.h"

class Model;

/**
 * Represents a single receiver.
 */
class Receiver {
public:
    // Constructor
    Receiver(QPoint pos);
    
    // Draws the receiver to the given pixels array
    void draw(QImage* pixels);
    
    // Get methods for representation variables
    QPoint getPos();
    
    // Set methods for representation variables
    void setPos(QPoint pos);
private:
    // Representation variables
    QPoint pos;
};

#endif