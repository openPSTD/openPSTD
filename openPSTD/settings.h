#ifndef SETTINGS_H
#define SETTINGS_H

#include <QColor>

class Settings {
public:
    // Constructor
    Settings() {}
    
    // Color of the background grid
    QRgb gridColor = qRgb(255, 0, 0);
    
    // Color of the window background
    QRgb bgColor = qRgb(255, 255, 255);
    
    // Color of the zoom level reference
    QRgb zoomColor = qRgb(0, 0, 0);
    
    // Clamping distance for the cursor
    int clampDist = 10;
    
    // Color of the fps counter
    QRgb fpsColor = qRgb(0, 0, 255);
    
    // Color of a source
    QRgb sourceColor = qRgb(255, 0, 0);
    
    // Color of a receiver
    QRgb receiverColor = qRgb(0, 255, 0);
    
    // Color of a wall depending on its absorption
    QRgb wallColor0 = qRgb(0, 0, 0);
    QRgb wallColor1 = qRgb(0, 255, 0);
};

#endif