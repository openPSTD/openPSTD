#ifndef DOMAIN_H
#define DOMAIN_H

#include <QImage>
#include <QPen>

/**
 * Representation of a single domain.
 */
class Domain {
public:
    // Constructor
    Domain(int x0, int y0, int x1, int y1);
    
    // Drawing method
    void draw(QImage* pixels);
    
    // Set methods for the domain's corner coordinates
    void setX1(int x1) { this->x1 = x1; }
    void setY1(int y1) { this->y1 = y1; }
private:
    // Corner coordinates
    int x0;
    int x1;
    int y0;
    int y1;
};

#endif