#include "scenevalidator.h"

/**
 * Validates that a given configuration is valid.
 * Will call exit(1) if the given configuration is invalid.
 * 
 * @param conf  The configuration to validate
 */
void SceneValidator::validateScene(Configuration* conf) {
	// Validate all rules one at a time
	hasDomain(conf);
	hasSource(conf);
	noOverlappingDomains(conf);
	domainsConnected(conf);
	sourcesInDomains(conf);
}

/**
 * Verifies that the given configuration has at least one domain.
 * 
 * @param conf  The configuration to validate
 */
void SceneValidator::hasDomain(Configuration* conf) {
	// Verify that there is at least one domain
	if (conf->domains.size() < 1) {
		std::cerr << "[SceneValidator] There must be at least one domain" << std::endl;
		exit(1);
	}
}

/**
 * Verifies that the given configuration has at least one source.
 * 
 * @param conf  The configuration to validate
 */
void SceneValidator::hasSource(Configuration* conf) {
	// Verify that there is at least one source
	if (conf->sources.size() < 1) {
		std::cerr << "[SceneValidator] There must be at least one source" << std::endl;
		exit(1);
	}
}

/**
 * Verifies that no two domains in the given configuration overlap.
 * 
 * @param conf  The configuration to validate
 */
void SceneValidator::noOverlappingDomains(Configuration* conf) {
	// Loop through all pairs of domains
	for (unsigned int i = 0; i < conf->domains.size(); i++) {
		// Get the coordinates of the first domain
		int x00 = conf->domains[i].left;
		int x01 = x00 + conf->domains[i].width;
		int y00 = conf->domains[i].bottom;
		int y01 = y00 + conf->domains[i].height;
		
		for (unsigned int j = i+1; j < conf->domains.size(); j++) {
			// Get the coordinates of the second domain
			int x10 = conf->domains[j].left;
			int x11 = x10 + conf->domains[j].width;
			int y10 = conf->domains[j].bottom;
			int y11 = conf->domains[j].height;
			
			// Check if these two domains overlap
			bool xmatch = (x00 < x11 && x10 < x01);
			bool ymatch = (y00 < y11 && y10 < y01);
			if (xmatch && ymatch) {
				std::cerr << "[SceneValidator] Domains cannot overlap" << std::endl;
				exit(1);
			}
		}
	}
}

/**
 * Verifies that all domains in the given configuration are connected.
 * 
 * @param conf  The configuration to validate
 */
void SceneValidator::domainsConnected(Configuration* conf) {
	// Initialize a set of all domain IDs that are connected
	std::set<unsigned int> connected;
	connected.insert(0);
	
	// Loop until no domains are added to connected anymore
	unsigned long oldSize = 0;
	while (connected.size() != oldSize) {
		// Update the oldSize
		oldSize = connected.size();
		
		// Loop through all domains
		for (unsigned int i = 0; i < conf->domains.size(); i++) {
			// Do nothing if this domain is already connected
			if (connected.find(i) != connected.end()) continue;
			
			// Get the coordinates of this domain
			int x00 = conf->domains[i].left;
			int x01 = x00 + conf->domains[i].width;
			int y00 = conf->domains[i].bottom;
			int y01 = y00 + conf->domains[i].height;
			
			// Check if this domain connects to any of the connected domains
			std::set<unsigned int>::iterator it;
			for (it = connected.begin(); it != connected.end(); it++) {
				// Get the coordinates of this domain
				int x10 = conf->domains[*it].left;
				int x11 = x10 + conf->domains[*it].width;
				int y10 = conf->domains[*it].bottom;
				int y11 = y10 + conf->domains[*it].height;
				
				// Check if the domain connects horizontally
				if ((x00 == x11 || x01 == x10) && (y01 > y10 && y11 > y00)) {
					connected.insert(i);
					break;
				}
				
				// Check if the domain connect vertically
				if ((y00 == y11 || y01 == y10) && (x01 > x10 && x11 > x00)) {
					connected.insert(i);
					break;
				}
			}
		}
	}
	
	// Verify that all domains are connected
	if (connected.size() != conf->domains.size()) {
		std::cerr << "\\[SceneValidator] All domains must be connected" << std::endl;
		exit(1);
	}
}

/**
 * Verifies that all sources in the given configuration are in a domain.
 * 
 * @param conf  The configuration to validate
 */
void SceneValidator::sourcesInDomains(Configuration* conf) {
	// Loop through all sources
	for (unsigned int i = 0; i < conf->sources.size(); i++) {
		// Get the coordinates of this source
		int x = conf->sources[i].position_x;
		int y = conf->sources[i].position_y;
		
		// Check if there is a domain that contains this source
		bool found = false;
		for (unsigned int j = 0; j < conf->domains.size(); j++) {
			// Get the coordinates of this domain
			int x0 = conf->domains[i].left;
			int x1 = x0 + conf->domains[i].width;
			int y0 = conf->domains[i].bottom;
			int y1 = y0 + conf->domains[i].height;
			
			// Check if this domain contains this source
			bool xmatch = (x0 <= x && x <= x1);
			bool ymatch = (y0 <= y && y <= y1);
			if (xmatch && ymatch) {
				found = true;
				break;
			}
		}
		
		// Verify that a domain contained this source
		if (!found) {
			std::cerr << "[SceneValidator] All sources must be in a domain (";
			std::cerr << i << ")" << std::endl;
			exit(1);
		}
	}
}