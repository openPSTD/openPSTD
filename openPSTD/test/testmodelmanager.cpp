#include "testmodelmanager.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestModelManager::TestModelManager(Window* window) {
    // Save reference to the main window locally
    this->window = window;
}

/**
 * Verifies that ModelManager::getInstance() does not
 * return a nullptr.
 */
void TestModelManager::testGetInstance() {
    // Get the ModelManager instance
    ModelManager* instance = ModelManager::getInstance();
    
    // Verify that this is not a nullptr
    QVERIFY(instance != nullptr);
}

/**
 * Verifies that ModelManager::saveState(),
 * ModelManager::undo(), and ModelManager::redo()
 * work correctly.
 */
void TestModelManager::testUndoRedo() {
    // Set initial values in Model
    Model::getInstance()->offset = QPoint(10, 20);
    Model::getInstance()->gridsize = 15;
    
    // Save the current model
    ModelManager::getInstance()->saveState();
    
    // Change some values in Model
    Model::getInstance()->offset = QPoint(50, 50);
    Model::getInstance()->gridsize = 20;
    
    // Save the current model again
    ModelManager::getInstance()->saveState();
    
    // Undo the changes
    ModelManager::getInstance()->undo();
    
    // Verify that the Model has been updated correctly
    QVERIFY(Model::getInstance()->offset == QPoint(10, 20));
    QVERIFY(Model::getInstance()->gridsize == 15);
    
    // Redo the changes
    ModelManager::getInstance()->redo();
    
    // Verify that the Model has been updated correctly
    QVERIFY(Model::getInstance()->offset == QPoint(50, 50));
    QVERIFY(Model::getInstance()->gridsize == 15);
}