#ifndef TESTMENUVIEW_H
#define TESTMENUVIEW_H

#include <QtTest/QtTest>
#include "window.h"

/**
 * Tests the functionality of the view menu.
 */
class TestMenuView : public QObject {
    Q_OBJECT
public:
    TestMenuView(Window* window);
private:
    Window* window;
private slots:
    void toggleFPS();
};

#endif