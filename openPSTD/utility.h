#ifndef UTILITY_H
#define UTILITY_H

#include <QPoint>
#include <QRect>
#include <QRgb>
#include <QImage>
#include "model.h"

class Utility {
public:
    // Draws a single pixel to a given pixels array
    static void drawPixel(QPoint pos, QRgb color, QImage* pixels);
    
    // Draws a single line to a given pixels array
    static void drawLine(QPoint p1, QPoint p2, QRgb color, QImage* pixels);
    
    // Draws a rectangle to a given pixels array
    static void drawRect(QRect rect, QRgb color, QImage* pixels);
    
    // Converts a point in screen coordinates to object coordinates
    static QPoint scr2obj(QPoint pos);
    
    // Converts a point in object coordinates to screen coordinates
    static QPoint obj2scr(QPoint pos);
};

#endif