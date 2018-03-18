/**
 * CHANGELOG:
 * - Major refactoring in EventHandler (added EventListener).
 * - Resolved bug with incorrect snapping distance.
 * 
 * TODO:
 * - Moving domains by dragging
 * - Moving walls by dragging
 * - Moving domains by coordinates
 * - Moving walls by coordinates
 */
#ifndef EVENTHANDLER2_H
#define EVENTHANDLER2_H

#include <QPoint>
#include "model.h"
#include "renderer.h"
#include "grid.h"
#include "absorptiondialog.h"
#include "coordinatedialog.h"

/**
 * Receives events from EventListener
 * through its public event handling
 * methods, and executes them.
 */
class EventHandler2 {
public:
    // Constructor
    EventHandler2(Model* model, Renderer* renderer, QImage* pixels, QWidget* parent);
    
    // Public event handling methods
    void deleteSelected();
    void clearSelection();
    void cancelNewDomain();
    void changeAbsorptionDialog();
    void moveToCenter();
    void addDomain(QPoint pos, bool stop);
    void addSource();
    void addReceiver();
    void selectStart(QPoint pos);
    void selectStop(QPoint pos, bool stop);
    void selectDomainStop(QPoint pos, bool stop);
    void selectWallStop(QPoint pos, bool stop);
    void measure(QPoint pos, bool stop);
    void removeMeasure();
    void moveStart();
    void moveStop(QPoint pos, bool stop);
    void moveCoordinates(QPoint pos);
private:
    // Reference variables
    Model* model;
    Renderer* renderer;
    QImage* pixels;
    QWidget* parent;
    
    // State variables
    bool addingDomain;
    bool selecting;
    QPoint selectstart;
    bool measuring;
    QLine measureLine;
    bool moving;
    QPoint moveDelta;
};

#endif