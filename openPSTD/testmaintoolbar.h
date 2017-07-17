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
    void addDomain();
};

#endif