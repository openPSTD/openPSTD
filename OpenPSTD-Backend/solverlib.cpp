#include "solverlib.h"
#include <iostream>

/**
 * Computes the border of a given vector of domains.
 * I.e. a set of lines corresponding to the domain wall segments for which
 * it is not shared with another domain.
 * 
 * @param domains  The vector of domains to compute the border of
 * @post  line.p1.x <= line.p2.x for all lines
 * @post  line.p1.y <= line.p2.y for all lines
 * @return  A list of lines, describing the lines on the border
 */
std::vector<Line> SolverLib::computeBorders(std::vector<Domain>* domains) {
	// Compute a set of line segments from all domain walls
	std::vector<Line> lines;
	for (unsigned int i = 0; i < domains->size(); i++) {
		// Compute the four corner coordinates of this domain
		int left = domains->at(i).left;
		int right = domains->at(i).left + domains->at(i).width;
		int top = domains->at(i).bottom + domains->at(i).height;
		int bottom = domains->at(i).bottom;
		
		// Add the four walls
		lines.push_back({{left, bottom}, {left, top}, LEFT});
		lines.push_back({{right, bottom}, {right, top}, RIGHT});
		lines.push_back({{left, top}, {right, top}, TOP});
		lines.push_back({{left, bottom}, {right, bottom}, BOTTOM});
	}
	
	// Loop through all pairs of lines
	for (unsigned int i = 0; i < lines.size(); i++) {
		for (unsigned int j = i+1; j < lines.size(); j++) {
			// Check if these two lines overlap in the x-direction
			bool isHorizontal1 = (lines[i].p1.y == lines[i].p2.y);
			bool isHorizontal2 = (lines[j].p1.y == lines[j].p2.y);
			if (isHorizontal1 && isHorizontal2 && lines[i].p1.y == lines[j].p1.y) {
				if (lines[i].p1.x < lines[j].p2.x && lines[j].p1.x < lines[i].p2.x) {
					// Add new lines for the non-overlapping segments
					if (lines[i].p1.x < lines[j].p1.x) {
						lines.push_back({
							{lines[i].p1.x, lines[i].p1.y},
							{lines[j].p1.x, lines[j].p1.y},
							lines[i].side
						});
					}
					if (lines[j].p1.x < lines[i].p1.x) {
						lines.push_back({
							{lines[j].p1.x, lines[j].p1.y},
							{lines[i].p1.x, lines[i].p1.y},
							lines[i].side
						});
					}
					if (lines[i].p2.x < lines[j].p2.x) {
						lines.push_back({
							{lines[i].p2.x, lines[i].p2.y},
							{lines[j].p2.x, lines[j].p2.y},
							lines[i].side
						});
					}
					if (lines[j].p2.x < lines[i].p2.x) {
						lines.push_back({
							{lines[j].p2.x, lines[j].p2.y},
							{lines[i].p2.x, lines[i].p2.y},
							lines[i].side
						});
					}
					
					// Remove both from the lines vector
					lines.erase(lines.begin() + j);
					lines.erase(lines.begin() + i);
					
					// Treat this line again
					i--;
					break;
				}
			}
			if (!isHorizontal1 && !isHorizontal2 && lines[i].p1.x == lines[j].p1.x) {
				if (lines[i].p1.y < lines[j].p2.y && lines[j].p1.y < lines[i].p2.y) {
					// Add new lines for the non-overlapping segments
					if (lines[i].p1.y < lines[j].p1.y) {
						lines.push_back({
							{lines[i].p1.x, lines[i].p1.y},
							{lines[j].p1.x, lines[j].p1.y},
							lines[i].side
						});
					}
					if (lines[j].p1.y < lines[i].p1.y) {
						lines.push_back({
							{lines[j].p1.x, lines[j].p1.y},
							{lines[i].p1.x, lines[i].p1.y},
							lines[i].side
						});
					}
					if (lines[i].p2.y < lines[j].p2.y) {
						lines.push_back({
							{lines[i].p2.x, lines[i].p2.y},
							{lines[j].p2.x, lines[j].p2.y},
							lines[i].side
						});
					}
					if (lines[j].p2.y < lines[i].p2.y) {
						lines.push_back({
							{lines[j].p2.x, lines[j].p2.y},
							{lines[i].p2.x, lines[i].p2.y},
							lines[i].side
						});
					}
					
					// Remove both from the lines vector
					lines.erase(lines.begin() + j);
					lines.erase(lines.begin() + i);
					
					// Treat this line again
					i--;
					break;
				}
			}
		}
	}
	
	// Return the result vector
	return lines;
}