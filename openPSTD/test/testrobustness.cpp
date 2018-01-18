#include "testrobustness.h"

/**
 * Runs several tests to test robustness.
 * 
 * @param numtests  The number of tests to run
 * @param window  A reference to the main window
 */
void TestRobustness::test(int numtests, Window* window) {
    // Use a random seed
    srand(time(NULL));
    
    // Loop numtests times
    for (int i = 0; i < numtests; i++) {
        // Generate a random model
        generateModel(window);
        
        // Verify that the model is in a valid state
        int result = verifyModel(window);
        
        // Output progress
        std::cout << "Test " << (i+1) << "/" << numtests << ": " << result << std::endl;
        
        // Stop if the model is in an invalid state
        if (result != 0) return;
    }
}

/**
 * Reruns the test in the log file.
 * 
 * @param window  A reference to the main window
 */
void TestRobustness::testLog(Window* window) {
    // Reset the model
    window->view->model = new Model();
    
    // Open the log file
    std::ifstream in("log");
    
    // Loop through all lines in the log file
    std::string line;
    while (std::getline(in, line)) {
        // Create a stringstream for this line
        if (line == "") continue;
        std::stringstream ss(line);
        
        // First word is actionID
        int actionID;
        ss >> actionID;
        
        // Second word is x
        int x;
        ss >> x;
        
        // Third word is y
        int y;
        ss >> y;
        
        // Fourth word is mouse drag
        bool d;
        ss >> d;
        
        // Fifth word is zoom level
        int z;
        ss >> z;
        
        // Sixth word is grid size
        int g;
        ss >> g;
        
        // Execute the action
        execAction(actionID, window, x, y, d, z, g * window->settings->pstdGridSize);
    }
    
    // Verify that the model is in a valid state
    int result = verifyModel(window);
    
    // Output result
    std::cout << "Log: " << result << std::endl;
}

/**
 * Generates a random model, on which robustness can be tested.
 * 
 * @param window  A reference to the main window
 */
void TestRobustness::generateModel(Window* window) {
    // Reset the model
    window->view->model = new Model();
    
    // Reset the log file
    std::ofstream out("log");
    
    // Generate a random number of actions to make
    int numactions = rand() % 100;
    
    // Loop numactions times
    for (int i = 0; i < numactions; i++) {
        // Execute a random action
        int actionID = rand() % 16;
        int x = rand() % 1024;
        int y = rand() % 768;
        bool d = (rand() % 2 == 0);
        int z = (rand() % 99) + 1;
        int g = (rand() % 9) + 1;
        
        // Output to the log file
        out << actionID << " " << x << " " << y << " " << d << " " << z << " " << g << std::endl;
        
        // Execute the action
        execAction(actionID, window, x, y, d, z, g * window->settings->pstdGridSize);
    }
}

/**
 * Verifies that the model is in a valid state.
 * 
 * @param window  A reference to the main window
 */
int TestRobustness::verifyModel(Window* window) {
    // Verify that no two domains overlap
    if (!verifyDomainOverlap(window)) return 1;
    
    // Verify that all domains are connected
    if (!verifyDomainConnect(window)) return 2;
    
    // Verify that all sources are inside some domain
    if (!verifySourceDomain(window)) return 3;
    
    // Verify that all receivers are inside some domain
    if (!verifyReceiverDomain(window)) return 4;
    
    // Verify that no two sources overlap
    if (!verifySourceOverlap(window)) return 5;
    
    // Verify that no two receivers overlap
    if (!verifyReceiverOverlap(window)) return 6;
    
    // Verify that no source and receiver overlap
    if (!verifySourceReceiver(window)) return 7;
    
    // Verify that the domain walls are correct
    if (!verifyDomainWalls(window)) return 8;
    
    // Model is valid
    return 0;
}

void TestRobustness::execAction(int actionID, Window* window, int x, int y, bool d, int z, int g) {
    if (actionID == 0) window->view->renderer->setState(SELECT);
    if (actionID == 1) window->view->renderer->setState(MOVE);
    if (actionID == 2) window->view->renderer->setState(ADDDOMAIN);
    if (actionID == 3) window->view->renderer->setState(ADDSOURCE);
    if (actionID == 4) window->view->renderer->setState(ADDRECEIVER);
    if (actionID == 5) window->view->renderer->setState(MEASURE);
    if (actionID == 6) window->view->undo();
    if (actionID == 7) window->view->redo();
    if (actionID == 8) window->view->renderer->moveToCenter();
    if (actionID == 9) window->view->renderer->eh->mousePress(x, y, Qt::LeftButton, Qt::NoModifier);
    if (actionID == 10) window->view->renderer->eh->mouseRelease(x, y, Qt::LeftButton);
    if (actionID == 11) window->view->renderer->eh->mouseDrag(x, y, d, Qt::NoModifier);
    if (actionID == 12) window->view->renderer->deleteSelected();
    if (actionID == 13) window->view->renderer->clearSelection();
    if (actionID == 14) window->slot_zoom(z);
    if (actionID == 15) window->view->renderer->setGridSize(g);
}

/**
 * Verifies that no two domains overlap.
 * 
 * @param window  A reference to the main window
 * @return  True iff no two domains overlap
 */
bool TestRobustness::verifyDomainOverlap(Window* window) {
    // Loop through all domains
    Model* model = window->view->model;
    for (int i = 0; i < model->domains.size(); i++) {
        // Get the QRect of this domain
        QRect recti = QRect(
            model->domains[i].getX0(),
            model->domains[i].getY0(),
            model->domains[i].getX1() - model->domains[i].getX0(),
            model->domains[i].getY1() - model->domains[i].getY0()
        );
        
        // Verify that this domain does not overlap with any other domain
        for (int j = 0; j < model->domains.size(); j++) {
            // Get the QRect of this domain
            QRect rectj = QRect(
                model->domains[j].getX0(),
                model->domains[j].getY0(),
                model->domains[j].getX1() - model->domains[j].getX0(),
                model->domains[j].getY1() - model->domains[j].getY0()
            );
            
            // Check if these domains overlap
            if (recti.intersects(rectj)) {
                return false;
            }
        }
    }
    
    // No two domains overlap
    return true;
}

/**
 * Verifies that all domains are connected together.
 * 
 * @param window  A reference to the main window
 * @return  True iff all domains are connected
 */
bool TestRobustness::verifyDomainConnect(Window* window) {
    // Do nothing if there are no domains
    Model* model = window->view->model;
    if (model->domains.size() == 0) return true;
    
    // Initialize vectors for connected and unconnected domains
    std::vector<unsigned int> connected;
    std::vector<unsigned int> unconnected;
    connected.push_back(0);
    for (unsigned int i = 1; i < model->domains.size(); i++) {
        unconnected.push_back(i);
    }
    
    // Loop until no domains are added to the connected vector anymore
    bool updated = true;
    while (updated) {
        // Reset the updated variable
        updated = false;
        
        // Loop through all unconnected domains
        std::vector<int> newconnections;
        for (unsigned int i = 0; i < unconnected.size(); i++) {
            // Get the rectangle of this domain
            QRect recti = QRect(
                model->domains[unconnected[i]].getX0(),
                model->domains[unconnected[i]].getY0(),
                model->domains[unconnected[i]].getX1() - model->domains[unconnected[i]].getX0(),
                model->domains[unconnected[i]].getY1() - model->domains[unconnected[i]].getY0()
            );
            
            // Loop through all connected domains
            for (unsigned int j = 0; j < connected.size(); j++) {
                // Get the rectangle of this domain
                QRect rectj = QRect(
                    model->domains[connected[j]].getX0(),
                    model->domains[connected[j]].getY0(),
                    model->domains[connected[j]].getX1() - model->domains[connected[j]].getX0(),
                    model->domains[connected[j]].getY1() - model->domains[connected[j]].getY0()
                );
                
                // Check if these two domains are connected
                if (recti.intersects(rectj)) {
                    newconnections.push_back(i);
                    break;
                }
            }
        }
        
        // Loop through the new connections
        for (unsigned int i = 0; i < newconnections.size(); i++) {
            // Remove this domain from unconnected
            unconnected.erase(std::find(unconnected.begin(), unconnected.end(), newconnections[i]));
            
            // Add this domain to connected
            connected.push_back(newconnections[i]);
            
            // Note that the connected vector has changed
            updated = true;
        }
    }
    
    // Verify that all domains are now in connected
    return (unconnected.size() == 0);
}

/**
 * Verifies that all sources are inside some domain.
 * 
 * @param window  A reference to the main window
 * @return  True iff all sources are inside some domain
 */
bool TestRobustness::verifySourceDomain(Window* window) {
    // Loop through all sources
    Model* model = window->view->model;
    for (int i = 0; i < model->sources.size(); i++) {
        // Get the position of this source
        QPoint pos = QPoint(
            model->sources[i].getX(),
            model->sources[i].getY()
        );
        
        // Loop through all domains
        bool inside = false;
        for (int j = 0; j < model->domains.size(); j++) {
            // Get the QRect of this domain
            QRect rect = QRect(
                model->domains[j].getX0(),
                model->domains[j].getY0(),
                model->domains[j].getX1() - model->domains[j].getX0(),
                model->domains[j].getY1() - model->domains[j].getY0()
            );
            
            // Check if the source is inside this domain
            if (rect.contains(pos, true)) {
                inside = true;
                break;
            }
        }
        
        // Verify that the source is inside some domain
        if (!inside) return false;
    }
    
    // All sources are inside some domain
    return true;
}

/**
 * Verifies that all receivers are inside some domain.
 * 
 * @param window  A reference to the main window
 * @return  True iff all receivers are inside some domain
 */
bool TestRobustness::verifyReceiverDomain(Window* window) {
    // Loop through all receivers
    Model* model = window->view->model;
    for (int i = 0; i < model->receivers.size(); i++) {
        // Get the position of this receiver
        QPoint pos = QPoint(
            model->receivers[i].getX(),
            model->receivers[i].getY()
        );
        
        // Loop through all domains
        bool inside = false;
        for (int j = 0; j < model->domains.size(); j++) {
            // Get the QRect of this domain
            QRect rect = QRect(
                model->domains[j].getX0(),
                model->domains[j].getY0(),
                model->domains[j].getX1() - model->domains[j].getX0(),
                model->domains[j].getY1() - model->domains[j].getY0()
            );
            
            // Check if the receiver is inside this domain
            if (rect.contains(pos, true)) {
                inside = true;
                break;
            }
        }
        
        // Verify that the receiver is inside some domain
        if (!inside) return false;
    }
    
    // All receivers are inside some domain
    return true;
}

/**
 * Verifies that no two sources overlap.
 * 
 * @param window  A reference to the main window
 * @return  True iff no two sources overlap
 */
bool TestRobustness::verifySourceOverlap(Window* window) {
    // Loop through all sources
    Model* model = window->view->model;
    for (int i = 0; i < model->sources.size(); i++) {
        // Get the position of this source
        QPoint posi = QPoint(
            model->sources[i].getX(),
            model->sources[i].getY()
        );
        
        // Loop through all sources
        for (int j = i+1; j < model->sources.size(); j++) {
            // Get the position of this source
            QPoint posj = QPoint(
                model->sources[j].getX(),
                model->sources[j].getY()
            );
            
            // Check if the two sources overlap
            if (posi == posj) return false;
        }
    }
    
    // No two sources overlap
    return true;
}

/**
 * Verifies that no two receivers overlap.
 * 
 * @param window  A reference to the main window
 * @return  True iff no two receivers overlap
 */
bool TestRobustness::verifyReceiverOverlap(Window* window) {
    // Loop through all receivers
    Model* model = window->view->model;
    for (int i = 0; i < model->receivers.size(); i++) {
        // Get the position of this receiver
        QPoint posi = QPoint(
            model->receivers[i].getX(),
            model->receivers[i].getY()
        );
        
        // Loop through all receivers
        for (int j = i+1; j < model->receivers.size(); j++) {
            // Get the position of this receiver
            QPoint posj = QPoint(
                model->receivers[j].getX(),
                model->receivers[j].getY()
            );
            
            // Check if the two receivers overlap
            if (posi == posj) return false;
        }
    }
    
    // No two receivers overlap
    return true;
}

/**
 * Verifies that no source overlaps with a receiver.
 * 
 * @param window  A reference to the main window
 * @return  True iff no source overlaps with a receiver
 */
bool TestRobustness::verifySourceReceiver(Window* window) {
    // Loop through all sources
    Model* model = window->view->model;
    for (int i = 0; i < model->sources.size(); i++) {
        // Get the position of this source
        QPoint posi = QPoint(
            model->sources[i].getX(),
            model->sources[i].getY()
        );
        
        // Loop through all receivers
        for (int j = 0; j < model->receivers.size(); j++) {
            // Get the position of this receiver
            QPoint posj = QPoint(
                model->receivers[j].getX(),
                model->receivers[j].getY()
            );
            
            // Check if the source and receiver overlap
            if (posi == posj) return false;
        }
    }
    
    // No source and receiver overlap
    return true;
}

/**
 * Verifies that each domains' walls are correct.
 * 
 * @param window  A reference to the main window
 * @return  True iff each domains' walls are correct
 */
bool TestRobustness::verifyDomainWalls(Window* window) {
    // Loop through all pixels
    QImage* pixels = window->view->renderer->pixels;
    Model* model = window->view->model;
    for (int j = 0; j < pixels->height(); j++) {
        for (int i = 0; i < pixels->width(); i++) {
            // Check if a domain is drawn here
            bool hasDomain = (pixels->pixel(i, j) == qRgb(0, 0, 0));
            
            // Loop through all domains
            int numdomains = 0;
            for (int k = 0; k < model->domains.size(); k++) {
                // Get the position of this domain
                QRect rect = QRect(
                    model->domains[k].getX0(),
                    model->domains[k].getY0(),
                    model->domains[k].getX1() - model->domains[k].getX0(),
                    model->domains[k].getY1() - model->domains[k].getY0()
                );
                int left = rect.left() * model->zoom + model->offsetX;
                int right = rect.right() * model->zoom + model->offsetX;
                int top = rect.top() * model->zoom + model->offsetY;
                int bottom = rect.bottom() * model->zoom + model->offsetY;
                
                // Check if (i, j) is on the border of this domain
                if (i == left || i == right || j == top || j == bottom) {
                    numdomains++;
                }
            }
            
            // Verify that the number of domains matches the drawn pixel
            if ((numdomains == 1) != hasDomain) {
                return false;
            }
        }
    }
    
    // All pixels are drawn correctly
    return true;
}