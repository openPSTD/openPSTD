#ifndef TESTMENUSCENE_H
#define TESTMENUSCENE_H

#include <QtTest/QtTest>
#include "window.h"

/**
 * Tests the functionality of the scene menu.
 */
class TestMenuScene : public QObject {
	Q_OBJECT
public:
	TestMenuScene(Window* window);
private:
	Window* window;
private slots:
	void clearScene();
};

#endif