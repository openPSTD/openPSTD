#ifndef TESTMODELMANAGER_H
#define TESTMODELMANAGER_H

#include <QtTest/QtTest>
#include "window.h"
#include "modelmanager.h"

class TestModelManager : public QObject {
    Q_OBJECT
public:
    TestModelManager(Window* window);
private:
    Window* window;
private slots:
    // Tests for ModelManager::getInstance()
    void testGetInstance();
    
    // Tests for saveState(), undo(), redo()
    void testUndoRedo();
};

#endif