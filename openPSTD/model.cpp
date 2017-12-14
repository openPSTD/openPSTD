#include "model.h"
#include "domain.h"
#include "source.h"
#include "receiver.h"

Model::Model() {}

Model::~Model() {
    for (unsigned int i = 0; i < domains.size(); i++) {
        delete domains[i];
    }
    
    for (unsigned int i = 0; i < sources.size(); i++) {
        delete sources[i];
    }
    
    for (unsigned int i = 0; i < receivers.size(); i++) {
        delete receivers[i];
    }
}

Model* Model::getInstance() {
    return ModelManager::getInstance()->getCurrent();
}