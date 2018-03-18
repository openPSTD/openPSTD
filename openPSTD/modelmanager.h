#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "model.h"

/**
 * Manages the current model by keeping track of
 * previous models, to provide functionality for
 * undo and redo operations.
 */
class ModelManager {
public:
    ModelManager(Model* current);
    ~ModelManager();
    
    void saveState();
    void undo();
    void redo();
private:
    std::vector<Model*> previous;
    Model* current;
    std::vector<Model*> next;
    
    Model* copyCurrent();
    void saveCurrent(Model* model);
};

#endif