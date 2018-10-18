#include "modelmanager.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::getInstance() {
	if (!instance) instance = new ModelManager();
	return instance;
}

ModelManager::ModelManager() {
	current = new Model();
	current->state = SELECT;
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
	
	updateWindowActions();
}

/**
 * Undo the last operation.
 */
void ModelManager::undo(bool allowRedo) {
	// Do nothing if there is no previous state
	if (previous.size() == 0) return;
	
	if (allowRedo) {
		// Create a copy of the current model
		Model* copy = copyCurrent();
		
		// Add this copy to the end of the next vector
		next.push_back(copy);
	}
	
	// Copy the previous model into the current model
	unsigned long index = previous.size() - 1;
	saveCurrent(previous[index]);
	
	// Remove the previous model from the previous vector
	delete previous[index];
	previous.pop_back();
	
	// Reset and re-merge all domain's walls
	for (unsigned int i = 0; i < current->domains.size(); i++) {
		current->domains[i]->resetWalls();
	}
	for (unsigned int i = 0; i < current->domains.size(); i++) {
		current->domains[i]->mergeDomains(&current->domains, i);
	}
	
	updateWindowActions();
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
	unsigned long index = next.size() - 1;
	saveCurrent(next[index]);
	
	// Remove the next model from the next vector
	delete next[index];
	next.pop_back();
	
	// Reset and re-merge all domain's walls
	for (unsigned int i = 0; i < current->domains.size(); i++) {
		current->domains[i]->resetWalls();
	}
	for (unsigned int i = 0; i < current->domains.size(); i++) {
		current->domains[i]->mergeDomains(&current->domains, i);
	}
	
	updateWindowActions();
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
		copy->domains.push_back(current->domains[i]->copy());
	}
	for (unsigned int i = 0; i < current->sources.size(); i++) {
		copy->sources.push_back(current->sources[i]->copy());
	}
	for (unsigned int i = 0; i < current->receivers.size(); i++) {
		copy->receivers.push_back(current->receivers[i]->copy());
	}
	copy->gridsize = current->gridsize;
	copy->zoom = current->zoom;
	copy->showFPS = current->showFPS;
	copy->actionUndo = current->actionUndo;
	copy->actionRedo = current->actionRedo;
	copy->actionChangeAbsorption = current->actionChangeAbsorption;
	
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
		current->domains.push_back(model->domains[i]->copy());
	}
	current->sources.clear();
	for (unsigned int i = 0; i < model->sources.size(); i++) {
		current->sources.push_back(model->sources[i]->copy());
	}
	current->receivers.clear();
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		current->receivers.push_back(model->receivers[i]->copy());
	}
	current->gridsize = model->gridsize;
	current->zoom = model->zoom;
	current->showFPS = model->showFPS;
	current->actionUndo = model->actionUndo;
	current->actionRedo = model->actionRedo;
	current->actionChangeAbsorption = model->actionChangeAbsorption;
}

void ModelManager::updateWindowActions() {
	bool hasPrev = previous.size() > 0;
	bool hasNext = next.size() > 0;
	
	current->actionUndo->setEnabled(hasPrev);
	current->actionRedo->setEnabled(hasNext);
}