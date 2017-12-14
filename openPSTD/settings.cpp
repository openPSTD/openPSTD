#include "settings.h"
#include <iostream>

Settings* Settings::instance = nullptr;

/**
 * Returns the instance of Settings.
 * 
 * @return  The instance of settings
 */
Settings* Settings::getInstance() {
    if (!instance) {
        instance = new Settings();
    }
    return instance;
}

/**
 * Private constructor.
 * Initializes all variables.
 */
Settings::Settings() {
    // Initialize all variables
    backgroundColor = qRgb(255, 255, 255);
    gridColor = qRgb(127, 180, 196);
    zoomRefColor = qRgb(0, 0, 0);
    fpsColor = qRgb(0, 0, 255);
    wallColor0 = qRgb(0, 0, 0);
    wallColor1 = qRgb(0, 255, 0);
    sourceColor = qRgb(255, 0, 0);
    receiverColor = qRgb(0, 255, 0);
    clampDist = 10;
    pstdGridSize = 16;
}