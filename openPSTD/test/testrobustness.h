#ifndef TESTROBUSTNESS_H
#define TESTROBUSTNESS_H

#include "window.h"

class TestRobustness {
public:
	static int test(int numtests, Window* window);
	static int testLog(Window* window);
private:
	static void generateModel(Window* window);
	static int verifyModel(Window* window);
	
	static int getAction();
	static void execAction(int actionID, Window* window, int x, int y, bool d, int z, int g);
	
	static bool verifyDomainOverlap(Window* window);
	static bool verifyDomainConnect(Window* window);
	static bool verifySourceDomain(Window* window);
	static bool verifyReceiverDomain(Window* window);
	static bool verifySourceOverlap(Window* window);
	static bool verifyReceiverOverlap(Window* window);
	static bool verifySourceReceiver(Window* window);
	static bool verifyDomainWalls(Window* window);
};

#endif