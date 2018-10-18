#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "model.h"
#include "domain.h"

/**
 * Manages the current model by keeping track of
 * previous models, to provide functionality for
 * undo and redo operations.
 */
class ModelManager {
public:
	//ModelManager(Model* current);
	~ModelManager();
	
	void saveState();
	void undo(bool allowRedo = true);
	void redo();
	
	bool canUndo() { return previous.size() > 0; }
	bool canRedo() { return next.size() > 0; }
	
	inline Model* getCurrent() { return current; }
	
	static ModelManager* getInstance();
private:
	std::vector<Model*> previous;
	Model* current;
	std::vector<Model*> next;
	
	ModelManager();
	static ModelManager* instance;
	
	Model* copyCurrent();
	void saveCurrent(Model* model);
	
	void updateWindowActions();
};

#endif