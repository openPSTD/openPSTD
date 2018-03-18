#ifndef TESTUNDOREDO_H
#define TESTUNDOREDO_H

#include <QtTest/QtTest>
#include "window.h"

/**
 * Tests the functionality of the undo and redo operations.
 */
class TestUndoRedo : public QObject {
    Q_OBJECT
public:
    TestUndoRedo(Window* window);
private:
    Window* window;
private slots:
    void addDomain();
    void addSource();
    void addReceiver();
};

#endif