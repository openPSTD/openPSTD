#include "modelmanager.h"

/**
 * Constructor.
 * 
 * @param current  A reference to the current model
 */
ModelManager::ModelManager(Model* current) {
    // Save reference variables locally
    this->current = current;
}

/**
 * Destructor.
 */
ModelManager::~ModelManager() {
    // Delete all previous models
    for (unsigned int i = 0; i < previous.size(); i++) {
        delete previous[i];
    }
    
    // Delete all next models
    for (unsigned int i = 0; i < next.size(); i++) {
        delete next[i];
    }
}

/**
 * Copies the current model into the previous vector.
 */
void ModelManager::saveState() {
    // Create a copy of the current model
    Model* copy = copyCurrent();
    
    // Add this copy to the end of the previous vector
    previous.push_back(copy);
    
    // Clear the next vector
    for (unsigned int i = 0; i < next.size(); i++) {
        delete next[i];
    }
    next.clear();
}

/**
 * Undo the last operation.
 */
void ModelManager::undo() {
    // Do nothing if there is no previous state
    if (previous.size() == 0) return;
    
    // Create a copy of the current model
    Model* copy = copyCurrent();
    
    // Add this copy to the end of the next vector
    next.push_back(copy);
    
    // Copy the previous model into the current model
    int index = previous.size() - 1;
    saveCurrent(previous[index]);
    
    // Remove the previous model from the previous vector
    delete previous[index];
    previous.pop_back();
    
    // Reset and re-merge all domain's walls
    for (unsigned int i = 0; i < current->domains.size(); i++) {
        current->domains[i].resetWalls();
    }
    for (unsigned int i = 0; i < current->domains.size(); i++) {
        current->domains[i].mergeDomains(&current->domains, i);
    }
}

/**
 * Redo the last operation.
 */
void ModelManager::redo() {
    // Do nothing if there is no next state
    if (next.size() == 0) return;
    
    // Create a copy of the current model
    Model* copy = copyCurrent();
    
    // Add this copy to the end of the previous vector
    previous.push_back(copy);
    
    // Copy the next model into the current model
    int index = next.size() - 1;
    saveCurrent(next[index]);
    
    // Remove the next model from the next vector
    delete next[index];
    next.pop_back();
    
    // Reset and re-merge all domain's walls
    for (unsigned int i = 0; i < current->domains.size(); i++) {
        current->domains[i].resetWalls();
    }
    for (unsigned int i = 0; i < current->domains.size(); i++) {
        current->domains[i].mergeDomains(&current->domains, i);
    }
}

/**
 * Creates a copy of the current model.
 * 
 * @return A copy of the current model.
 */
Model* ModelManager::copyCurrent() {
    // Create a new Model instance
    Model* copy = new Model();
    
    // Copy all state variables from the current model
    copy->state = current->state;
    for (unsigned int i = 0; i < current->domains.size(); i++) {
        copy->domains.push_back(Domain(
            current->domains[i].getX0(),
            current->domains[i].getY0(),
            current->domains[i].getX1(),
            current->domains[i].getY1(),
            current->domains[i].getSettings()
        ));
    }
    copy->sources = current->sources;
    copy->receivers = current->receivers;
    copy->gridsize = current->gridsize;
    copy->zoom = current->zoom;
    copy->showFPS = current->showFPS;
    
    // Return the copy
    return copy;
}

/**
 * Saves the given model as current model
 * 
 * @param model  The model to use as current
 */
void ModelManager::saveCurrent(Model* model) {
    // Copy all state variables from the given model to the current model
    current->state = model->state;
    current->domains.clear();
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        current->domains.push_back(Domain(
            model->domains[i].getX0(),
            model->domains[i].getY0(),
            model->domains[i].getX1(),
            model->domains[i].getY1(),
            model->domains[i].getSettings()
        ));
    }
    current->sources = model->sources;
    current->receivers = model->receivers;
    current->gridsize = model->gridsize;
    current->zoom = model->zoom;
    current->showFPS = model->showFPS;
}