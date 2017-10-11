#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QInputDialog>
#include <QGraphicsScene>
#include <sstream>
#include <iomanip>
#include "model.h"
#include "state.h"
#include "settings.h"
#include "modelmanager.h"
#include "grid.h"
#include "source.h"

/**
 * Event handling class.
 * Receives mouse and keyboard events from Renderer,
 * and determines what to do.
 */
class EventHandler {
public:
    // Constructor
    EventHandler(Model* model, Settings* settings, ModelManager* modelmanager, QWidget* parent);
    
    // Public event handling methods
    void mousePress(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    void mouseRelease(int x, int y, Qt::MouseButton button);
    void mouseDrag(int x, int y, bool drag, Qt::KeyboardModifiers modifiers);
    void doubleClick(int x, int y, Qt::MouseButton button);
    
    // Get method for current mouse position
    QPoint getMousePos() { return QPoint(mouseX, mouseY); }
    
    // Get methods for selected objects
    bool isSourceSelected(unsigned int i);
    bool isReceiverSelected(unsigned int i);
    std::vector<unsigned int> getSelectedWalls(unsigned int domainID);
    
    // Drawing methods
    void drawSelection(QImage* pixels);
    void drawMeasure(QImage* pixels);
    void drawOverlap(QImage* pixels);
    
    // Deletes all selected objects
    void deleteSelected();
    void clearSelection();
    inline void removeMeasure() { measuring = false; }
private:
    // Class instance variables
    Model* model;
    Settings* settings;
    ModelManager* modelmanager;
    QWidget* parent;
    
    // State variables
    bool addingDomain;
    int mouseX;
    int mouseY;
    int moveSceneX;
    int moveSceneY;
    std::pair<int, int> selectStart;
    std::pair<int, int> selectEnd;
    bool selecting;
    std::vector<std::pair<unsigned int, unsigned int>> selectedWalls;
    std::vector<unsigned int> selectedSources;
    std::vector<unsigned int> selectedReceivers;
    std::pair<int, int> measureStart;
    std::pair<int, int> measureEnd;
    bool measuring;
    bool overlap;
    
    // Private event handling methods
    void addDomainStart(int x, int y);
    void addDomainStop(int x, int y);
    void addSource(int x, int y);
    void addReceiver(int x, int y);
    void select(bool ctrl);
    
    // Private drawing methods
    void drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels);
    
    // Update method for walls' length text state
    void updateWallTextState();
};

#endif