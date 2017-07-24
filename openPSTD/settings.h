#ifndef SETTINGS_H
#define SETTINGS_H

#include <QColor>

// Color of the background grid
static const QRgb gridColor = qRgb(255, 0, 0);

// Color of the window background
static const QRgb bgColor = qRgb(255, 255, 255);

// Color of the add domain cursor
static const QRgb cursorColor = qRgb(0, 255, 0);

// Clamping distance for the cursor
static const int clampDist = 10;

// Color of the zoom level reference
static const QRgb zoomColor = qRgb(0, 0, 0);

static const QRgb fpsColor = qRgb(0, 0, 255);

#endif