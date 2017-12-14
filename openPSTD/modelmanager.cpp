#include "modelmanager.h"
#include "model.h"
#include "domain.h"
#include "source.h"
#include "receiver.h"

ModelManager* ModelManager::instance = 0;

/**
 * Get method for the ModelManager instance.
 * 
 * @return  The ModelManager instance
 */
ModelManager* ModelManager::getInstance() {
    // Create a ModelManager instance if there is none yet
    if (!instance) {
        instance = new ModelManager();
    }
    
    // Return the ModelManager instance
    return instance;
}

/**
 * Get method for the current Model instance.
 * To be called through Model::getCurrent().
 * 
 * @return  The current Model instance
 */
Model* ModelManager::getCurrent() {
    // Return the current Model
    return current;
}

/**
 * Destructor.
 */
ModelManager::~ModelManager() {
    // Delete all prev models
    for (unsigned int i = 0; i < prev.size(); i++) {
        delete prev[i];
    }
    
    // Delete all next models
    for (unsigned int i = 0; i < next.size(); i++) {
        delete next[i];
    }
    
    // Delete the current Model instance
    delete current;
    
    // Delete the ModelManager instance
    delete instance;
}

/**
 * Saves the current state of the Model to the stack.
 */
void ModelManager::saveState() {
    // Copy the current Model instance
    Model* c = copy();
    
    // Push the current Model instance to the prev stack
    prev.push_back(c);
    
    // Clear the next vector
    for (unsigned int i = 0; i < next.size(); i++) {
        delete next[i];
    }
    next.clear();
}

/**
 * Undo the last change.
 */
void ModelManager::undo() {
    // Do nothing if there is no prev state
    if (prev.size() == 0) return;
    
    // Create a copy of the current Model
    Model* c = copy();
    
    // Push the copy to the next vector
    next.push_back(c);
    
    // Copy the previous Model into the current Model
    save(prev.back());
    
    // Remove the prev Model from the prev vector
    delete prev.back();
    prev.pop_back();
}

/**
 * Redo the last change.
 */
void ModelManager::redo() {
    // Do nothing if there is no next state
    if (next.size() == 0) return;
    
    // Create a copy of the current Model
    Model* c = copy();
    
    // Push the copy to the prev vector
    prev.push_back(c);
    
    // Copy the next Model into the current Model
    save(next.back());
    
    // Remove the next Model from the next vector
    delete next.back();
    next.pop_back();
}

/**
 * Private constructor.
 */
ModelManager::ModelManager() {
    // Create a Model instance
    current = new Model();
}

/**
 * Creates a copy of the current Model.
 * 
 * @return  A copy of the current Model
 */
Model* ModelManager::copy() {
    // Create a new Model instance
    Model* c = new Model();
    
    // Copy all state variables from the current Model
    c->state     = current->state;
    c->domains   = current->domains;
    c->sources   = current->sources;
    c->receivers = current->receivers;
    c->gridsize  = current->gridsize;
    c->zoomlevel = current->zoomlevel;
    c->offset    = current->offset;
    c->showFPS   = current->showFPS;
    c->showGrid  = current->showGrid;
    
    // Return the copy
    return c;
}

/**
 * Saves the given Model as the current Model.
 * 
 * @param model  The Model to save
 */
void ModelManager::save(Model* model) {
    // Copy all state variables into the current Model
    current->state     = model->state;
    current->domains   = model->domains;
    current->sources   = model->sources;
    current->receivers = model->receivers;
    current->gridsize  = model->gridsize;
    current->zoomlevel = model->zoomlevel;
    current->offset    = model->offset;
    current->showFPS   = model->showFPS;
    current->showGrid  = model->showGrid;
}