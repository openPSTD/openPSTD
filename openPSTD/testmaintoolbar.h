#ifndef TESTMAINTOOLBAR_H
#define TESTMAINTOOLBAR_H

#include <QtTest/QtTest>
#include "window.h"

/**
 * Tests the functionality of the main toolbar.
 */
class TestMainToolbar : public QObject {
    Q_OBJECT
public:
    TestMainToolbar(Window* window);
private:
    Window* window;
private slots:
    void select();
    void move();
    void addDomain();
    void addSource();
    void addReceiver();
    void zoom();
    void gridSize();
};

#endif