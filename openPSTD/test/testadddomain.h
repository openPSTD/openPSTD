#ifndef TESTADDDOMAIN_H
#define TESTADDDOMAIN_H

#include <QtTest/QtTest>
#include "window.h"

/**
 * Tests the functionality of the add domain state.
 */
class TestAddDomain : public QObject {
	Q_OBJECT
public:
	TestAddDomain(Window* window);
private:
	Window* window;
	void checkDomain(int x0, int y0, int x1, int y1, Domain domain);
private slots:
	void addQ1();
	void addQ2();
	void addQ3();
	void addQ4();
};

#endif