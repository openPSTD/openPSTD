#include "testmenuscene.h"

/**
 * Constructor.
 * 
 * @param window  A reference to the main window
 */
TestMenuScene::TestMenuScene(Window* window) {
    // Save reference variables locally
    this->window = window;
}

/**
 * Verifies that the model is updated correctly once
 * the clear scene action is triggered.
 */
void TestMenuScene::clearScene() {
    // Add a new domain
    window->ui->actionAddDomain->activate(QAction::Trigger);
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(100, 100));
    QTest::mouseClick(window->view->viewport(), Qt::LeftButton, Qt::KeyboardModifiers(), QPoint(200, 200));
    QVERIFY(window->view->model->domains.size() > 0);
    
    // Trigger the clear scene action
    window->ui->actionClear_all->activate(QAction::Trigger);
    
    // Verify that the model contains no domains
    QVERIFY(window->view->model->domains.size() == 0);
}