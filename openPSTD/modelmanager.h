#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <vector>

class Model;

/**
 * Provides undo - redo functionality for the Model class.
 */
class ModelManager {
public:
    // Get method for the ModelManager instance
    static ModelManager* getInstance();
    
    // Get method for the current Model instance
    Model* getCurrent();
    
    // Destructor
    ~ModelManager();
    
    // Saves the current state of the Model to the stack
    void saveState();
    
    // Undo - redo the last change
    void undo();
    void redo();
private:
    // Private constructor
    ModelManager();
    
    // ModelManager instance
    static ModelManager* instance;
    
    // Current, previous and next Model stacks
    Model* current;
    std::vector<Model*> prev;
    std::vector<Model*> next;
    
    // Creates a copy of the current Model
    Model* copy();
    
    // Saves the given Model as the current Model
    void save(Model* model);
};

#endif