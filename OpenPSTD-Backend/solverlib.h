#ifndef SOLVERLIB_H
#define SOLVERLIB_H

#include <vector>
#include <algorithm>
#include "configuration.h"

enum Side {
	TOP,
	LEFT,
	BOTTOM,
	RIGHT
};

struct Point {
	int x;
	int y;
};

struct Line {
	Point p1;
	Point p2;
	Side side;
};

/**
 * A library for the Solver class, containing various generic algorithms.
 */
class SolverLib {
public:
	// Computes the border of a given vector of domains.
	static std::vector<Line> computeBorders(std::vector<Domain>* domains);
};

#endif