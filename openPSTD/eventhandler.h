#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <algorithm>
#include <QPoint>
#include <QRect>
#include <QLine>
#include "domain.h"
#include "wall.h"
#include "source.h"
#include "receiver.h"
#include "modelmanager.h"

/**
 * Executes specific functions after a received event
 * from EventListener.
 */
class EventHandler {
public:
    // Constructor, destructor
    EventHandler();
    
    // Event handling methods
    void startDomain(QPoint pos);
    void stopDomain(QPoint pos);
    void startSelection(QPoint pos);
    void stopSelection(QPoint pos);
    void deleteSelected();
    void clearSelection();
    void moveObject(QPoint pos);
    void editProperties(QPoint pos);
    void setState(State state);
    
    // Get methods for state variables
    QPoint getMousePos();
    QRect getSelection();
    QLine getMeasure();
    
    // Set methods for state variables
    void setMousePos(QPoint pos);
private:
    // Private event handling methods
    void moveDomain(Domain* domain, QPoint pos);
    void moveWall(Wall* wall, QPoint pos);
    void moveSource(Source* source, QPoint pos);
    void moveReceiver(Receiver* receiver, QPoint pos);
    
    // State variables
    QPoint mousePos;
    QRect selection;
    bool selecting;
    std::vector<int> selectedDomains;
    std::vector<std::pair<int, int>> selectedWalls;
    std::vector<int> selectedSources;
    std::vector<int> selectedReceivers;
    QLine measure;
    bool measuring;
};

#endif