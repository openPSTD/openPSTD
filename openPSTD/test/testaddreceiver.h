#ifndef TESTADDRECEIVER_H
#define TESTADDRECEIVER_H

#include <QtTest/QtTest>
#include "window.h"

/**
 * Tests the functionality of the add receiver state.
 */
class TestAddReceiver : public QObject {
    Q_OBJECT
public:
    TestAddReceiver(Window* window);
private:
    Window* window;
private slots:
    void addReceiver();
};

#endif