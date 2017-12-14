#ifndef SOURCE_H
#define SOURCE_H

#include <stdexcept>
#include <QPoint>
#include <QImage>
#include "utility.h"
#include "settings.h"
#include "modelmanager.h"

class Model;

/**
 * Represents a single source.
 */
class Source {
public:
    // Constructor
    Source(QPoint pos);
    
    // Draws the source to the given pixels array
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