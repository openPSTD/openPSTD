#ifndef SCENEVALIDATOR_H
#define SCENEVALIDATOR_H

#include <set>
#include "configuration.h"
#include <iostream>

/**
 * Validates a configuration.
 */
class SceneValidator {
public:
	// Validates the given configuration.
	static void validateScene(Configuration* conf);
private:
	// Private validation methods
	static void hasDomain(Configuration* conf);
	static void hasSource(Configuration* conf);
	static void noOverlappingDomains(Configuration* conf);
	static void domainsConnected(Configuration* conf);
	static void sourcesInDomains(Configuration* conf);
};

#endif