#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <QtTest/QtTest>
#include "window.h"
#include "model.h"

/**
 * Tests the functionality of the public methods
 * in the Model class.
 */
class TestModel : public QObject {
    Q_OBJECT
public:
    TestModel(Window* window);
private:
    Window* window;
    Model* model;
private slots:
    void clampGrid();
    void isOnGrid();
};

#endif