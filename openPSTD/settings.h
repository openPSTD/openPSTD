#ifndef SETTINGS_H
#define SETTINGS_H

#include <QRgb>

/**
 * Representation of the current settings.
 */
class Settings {
public:
    // Get method for the settings instance
    static Settings* getInstance();
    
    // Color of the scene backgroind
    QRgb backgroundColor;
    
    // Color of the background grid
    QRgb gridColor;
    
    // Color of the zoom reference
    QRgb zoomRefColor;
    
    // Color of the FPS monitor
    QRgb fpsColor;
    
    // Color range of a wall regarding its absorption
    QRgb wallColor0;
    QRgb wallColor1;
    
    // Color of a source
    QRgb sourceColor;
    
    // Color of a receiver
    QRgb receiverColor;
    
    // Clamp distance in pixels
    unsigned int clampDist;
    
    // PSTD grid size
    unsigned int pstdGridSize;
private:
    // Private constructor
    Settings();
    
    // Settings instance
    static Settings* instance;
};

#endif