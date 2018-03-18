#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include <QEvent>
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "state.h"
#include "model.h"
#include "renderer.h"
#include "eventhandler2.h"
#include <iostream>

/**
 * Defines a single key mapping.
 */
struct KeyMap {
    int button; // RMB=-3, MMB=-2, LMB=-1, a=0, ...
    Qt::KeyboardModifiers modifiers;
    int operation; // 0: Press, 1: Mouse move, 2: Release, 3: Double click
    State tool;
    std::string function;
};

/**
 * Receives events through its public methods, and delegates them
 * to the corresponding method in EventHandler according to keymaps.txt.
 * 
 * Guide for keymaps.txt:
 * Each line needs to follow the format "button,modifiers,operation,tool,function".
 * For button, the following values are allowed:
 *   LMB (left mouse button)
 *   MMB (middle mouse button)
 *   RMB (right mouse button)
 *   lowercase characters a through z
 * For modifiers, the following values are allowed:
 *   <empty> (no modifiers)
 *   shift
 *   control
 *   alt
 * To register multiple modifiers, split them with a pipe character.
 * In this case, all modifiers need to be pressed for the keymap to match.
 * For operation, the following values are allowed:
 *   0 (press)
 *   1 (mouse move)
 *   2 (release)
 *   3 (mouse wheel)
 *   4 (double click)
 */
class EventListener {
public:
    // Constructor, destructor
    EventListener(Model* model, QImage* pixels, QWidget* parent, Renderer* renderer);
    ~EventListener();
    
    // Public event methods
    void mousePress(QPoint mouse, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    void mouseMove(QPoint mouse, Qt::KeyboardModifiers modifiers);
    void mouseRelease(QPoint mouse, Qt::MouseButton button);
    void mouseWheel(QPoint mouse, int dx, int dy);
    void doubleClick(QPoint mouse, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
    void keyPress(int key, Qt::KeyboardModifiers modifiers);
    
    // Get method for EventHandler instance
    EventHandler2* getEventHandler() { return eh; }
    
    // Get method for the mouse position
    QPoint getMouse() { return mouse; }
private:
    // Reference variables
    Model* model;
    Renderer* renderer;
    EventHandler2* eh;
    
    // Vector of all registered keymaps
    std::vector<KeyMap> keymaps;
    
    // State variables
    QPoint mouse; // Mouse posiiton
    QPoint oldmouse; // Previous mouse position
    int wheelx; // Mouse wheel dx
    int wheely; // Mouse wheel dy
    
    // Loads all keymaps from keymaps.txt into the vector
    void loadKeymaps();
    
    // Converts a line from keymaps.txt into a KeyMap instance
    KeyMap lineToKeymap(std::string line);
    
    // Private conversion methods
    State toState(std::string s);
    Qt::KeyboardModifiers toModifiers(std::string s);
    int buttonToInt(Qt::MouseButton button);
    int keyToInt(std::string s);
    int keyToInt(int key);
    
    // Calls a method in EventHandler according to the given function
    void executeFunction(std::string function);
};

#endif