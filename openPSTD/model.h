#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "state.h"
#include "domain.h"

/**
 * Representation of the current scene.
 */
class Model {
public:
    // Constructor
    Model() {}
    
    // State of the main tool bar buttons
    State state;
    
    // Vector of all domains
    std::vector<Domain> domains;
    Domain* lastDomain() { return &domains[domains.size() - 1]; }
};

#endif