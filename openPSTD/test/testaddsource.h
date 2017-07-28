#ifndef TESTADDSOURCE_H
#define TESTADDSOURCE_H

#include <QtTest/QtTest>
#include "window.h"

/**
 * Tests the functionality of the add source state.
 */
class TestAddSource : public QObject {
public:
    TestAddSource(Window* window);
private:
    Window* window;
private slots:
    void addSource();
};

#endif