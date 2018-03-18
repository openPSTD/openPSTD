#include "eventlistener.h"

/**
 * Constructor.
 * Loads all keymaps from keymaps.txt into the vector.
 * 
 * @param model  A reference to the model
 * @param pixels  A reference to the QImage from Renderer
 * @param parent  A reference to the main window
 * @param renderer  A reference to the renderer
 */
EventListener::EventListener(Model* model, QImage* pixels, QWidget* parent, Renderer* renderer) {
    // Save the reference variables
    this->model = model;
    this->renderer = renderer;
    
    // Create a new EventHandler instance
    eh = new EventHandler2(model, renderer, pixels, parent);
    
    // Load all keymaps into the vector
    loadKeymaps();
}

/**
 * Destructor.
 */
EventListener::~EventListener() {
    // Delete the EventHandler instance
    delete eh;
}

/**
 * Public event method: Mouse press.
 * 
 * @param mouse  The position of the mouse
 * @param button  The mouse button that was pressed
 * @param modifiers  The keyboard modifiers of the event
 */
void EventListener::mousePress(QPoint mouse, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) {
    // Save the new mouse position
    this->mouse = mouse;
    
    // Loop through all registered keymaps
    for (unsigned int i = 0; i < keymaps.size(); i++) {
        // Verify that all elements of the keymap match the current event
        if (keymaps[i].button != buttonToInt(button)) continue;
        if (keymaps[i].modifiers != modifiers) continue;
        if (keymaps[i].operation != 0) continue;
        if (keymaps[i].tool != model->state) continue;
        
        // If so, execute the corresponding function
        executeFunction(keymaps[i].function);
    }
}

/**
 * Public event method: Mouse move.
 * 
 * @param mouse  The position of the mouse
 * @param modifiers  The keyboard modifiers of the event
 */
void EventListener::mouseMove(QPoint mouse, Qt::KeyboardModifiers modifiers) {
    // Save the new mouse position
    this->oldmouse = this->mouse;
    this->mouse = mouse;
    
    // Pass the new mouse position to Renderer
    renderer->setMouse(mouse);
    
    // Loop through all registered keymaps
    for (unsigned int i = 0; i < keymaps.size(); i++) {
        // Verify that all elements of the keymap match the current event
        if (keymaps[i].modifiers != modifiers) continue;
        if (keymaps[i].operation != 1) continue;
        if (keymaps[i].tool != model->state) continue;
        
        // If so, execute the corresponding function
        executeFunction(keymaps[i].function);
    }
}

/**
 * Public event method: Mouse release.
 * 
 * @param mouse  The position of the mouse
 * @param button  The mouse button that was released
 */
void EventListener::mouseRelease(QPoint mouse, Qt::MouseButton button) {
    // Save the new mouse position
    this->mouse = mouse;
    
    // Loop through all registered keymaps
    for (unsigned int i = 0; i < keymaps.size(); i++) {
        // Verify that all elements of the keymap match the current event
        if (keymaps[i].button != buttonToInt(button)) continue;
        if (keymaps[i].operation != 2) continue;
        if (keymaps[i].tool != model->state) continue;
        
        // If so, execute the corresponding function
        executeFunction(keymaps[i].function);
    }
}

/**
 * Public event method: Mouse wheel.
 * 
 * @param mouse  The position of the mouse
 * @param dx  The delta angle along the x-axis
 * @param dy  The delta angle along the y-axis
 */
void EventListener::mouseWheel(QPoint mouse, int dx, int dy) {
    // Save the new mouse position
    this->mouse = mouse;
    
    // Save the mouse wheel delta
    this->wheelx = dx;
    this->wheely = dy;
    
    // Loop through all registered keymaps
    for (unsigned int i = 0; i < keymaps.size(); i++) {
        // Verify that all elements of the keymap match the current event
        if (keymaps[i].operation != 3) continue;
        if (keymaps[i].tool != model->state) continue;
        
        // If so, execute the corresponding function
        executeFunction(keymaps[i].function);
    }
}

/**
 * Public event method: Double click.
 * 
 * @param mouse  The position of the mouse
 * @param button  The mouse button that was used
 * @param modifiers  The keyboard modifiers of the event
 */
void EventListener::doubleClick(QPoint mouse, Qt::MouseButton button, Qt::KeyboardModifiers modifiers) {
    // Save the new mouse position
    this->mouse = mouse;
    
    // Loop through all registered keymaps
    for (unsigned int i = 0; i < keymaps.size(); i++) {
        // Verify that all elements of the keymap match the current event
        if (keymaps[i].button != buttonToInt(button)) continue;
        if (keymaps[i].modifiers != modifiers) continue;
        if (keymaps[i].operation != 4) continue;
        if (keymaps[i].tool != model->state) continue;
        
        // If so, execute the corresponding function
        executeFunction(keymaps[i].function);
    }
}

/**
 * Public event method: Key press.
 * 
 * @param key  The key that was pressed
 * @param modifiers  The keyboard modifiers of the event
 */
void EventListener::keyPress(int key, Qt::KeyboardModifiers modifiers) {
    // Loop through all registered keymaps
    for (unsigned int i = 0; i < keymaps.size(); i++) {
        // Verify that all elements of the keymap match the current event
        if (keymaps[i].button != keyToInt(key)) continue;
        if (keymaps[i].modifiers != modifiers) continue;
        if (keymaps[i].operation != 0) continue;
        if (keymaps[i].tool != model->state) continue;
        
        // If so, execute the corresponding function
        executeFunction(keymaps[i].function);
    }
}

/**
 * Loads all keymaps in keymaps.txt into the vector.
 */
void EventListener::loadKeymaps() {
    // Open the keymaps file for reading
    QFile file(":/new/prefix1/keymaps.txt");
    file.open(QFile::ReadOnly | QFile::Text);
    
    // Loop through all lines in the file
    QTextStream ts(&file);
    while (!ts.atEnd()) {
        // Get the current line
        std::string line = ts.readLine().toStdString();
        
        // Skip this line if it is a comment
        if (line.substr(0, 2) == "//" || line == "") continue;
        
        // Convert the line to a KeyMap
        KeyMap keymap = lineToKeymap(line);
        
        // Add this KeyMap to the vector
        keymaps.push_back(keymap);
    }
    
    // Close the file
    file.close();
}

/**
 * Converts a single line from keymaps.txt to a KeyMap.
 * 
 * @param line  The line to convert
 * @return  The corresponding KeyMap
 */
KeyMap EventListener::lineToKeymap(std::string line) {
    // Create a stringstream for the line
    std::stringstream ss(line);
    
    // First element is the button
    std::string button;
    std::getline(ss, button, ',');
    
    // Second element is the keyboard modifiers
    std::string modifiers;
    std::getline(ss, modifiers, ',');
    
    // Third element is the operation
    int operation;
    ss >> operation;
    ss.ignore();
    
    // Fourth element is the tool
    std::string tool;
    std::getline(ss, tool, ',');
    
    // Fifth element is the function to execute
    std::string function;
    std::getline(ss, function, ',');
    
    // Create a KeyMap from the data
    KeyMap keymap;
    keymap.button = keyToInt(button);
    keymap.modifiers = toModifiers(modifiers);
    keymap.operation = operation;
    keymap.tool = toState(tool);
    keymap.function = function;
    
    // Return the KeyMap
    return keymap;
}

/**
 * Private conversion method: String to State.
 * 
 * @param s  A stringified State
 * @return  The corresponding State
 */
State EventListener::toState(std::string s) {
    // Check which of the states matches
    if (s == "SELECTDOMAIN") return SELECTDOMAIN;
    if (s == "SELECTWALL") return SELECTWALL;
    if (s == "SELECT") return SELECT;
    if (s == "MOVE") return MOVE;
    if (s == "ADDDOMAIN") return ADDDOMAIN;
    if (s == "ADDSOURCE") return ADDSOURCE;
    if (s == "ADDRECEIVER") return ADDRECEIVER;
    if (s == "MEASURE") return MEASURE;
    
    // If none match, return the Select state
    return SELECT;
}

/**
 * Private conversion method: String to Qt::KeyboardModifiers.
 * 
 * @param s  A string containing all modifiers, separated by a pipe character
 * @return  The corresponding Qt::KeyboardModifiers
 */
Qt::KeyboardModifiers EventListener::toModifiers(std::string s) {
    // Start with no modifiers
    Qt::KeyboardModifiers modifiers = Qt::NoModifier;
    
    // Loop through all modifiers (split on a pipe character)
    std::stringstream ss(s);
    std::string line;
    while (std::getline(ss, line, '|')) {
        // Add the modifier to modifiers
        if (line == "") continue;
        if (line == "shift") modifiers |= Qt::ShiftModifier;
        if (line == "control") modifiers |= Qt::ControlModifier;
        if (line == "alt") modifiers |= Qt::AltModifier;
    }
    
    // Return the result
    return modifiers;
}

/**
 * Private conversion method: Qt::MouseButton to int.
 * RMB: -3, MMB: -2, LMB: -1
 * 
 * @param button  The MouseButton instance to convert
 * @return  The corresponding integer
 */
int EventListener::buttonToInt(Qt::MouseButton button) {
    // Check which button matches
    if (button == Qt::LeftButton) return -1;
    if (button == Qt::MiddleButton) return -2;
    if (button == Qt::RightButton) return -3;
    
    // If none match, return the left mouse button
    return -1;
}

/**
 * Private conversion method: String to int.
 * a: 0, b: 1, ..., z: 25, esc: 26.
 * 
 * @param s  The string to convert
 * @return  The corresponding integer
 */
int EventListener::keyToInt(std::string s) {
    // Check if the string is a single character
    if (s.length() == 1) {
        // Cast it to an integer, and offset to get a: 0
        return (int) (s[0]) - 'a';
    }
    
    // Check if s is LMB, MMB, or RMB
    if (s == "LMB") return -1;
    if (s == "MMB") return -2;
    if (s == "RMB") return -3;
    
    // Check if key is ESC
    if (s == "ESC") return 26;
    
    // Unknown key, return LMB
    return -1;
}

/**
 * Private conversion method: Qt::Key to int.
 * Qt::Key_A: 0, Qt::Key_B: 1, ..., Qt::Key_Z: 25, Qt::Key_ESC: 26.
 * 
 * @param key  The Qt::Key to convert
 * @return  The corresponding integer
 */
int EventListener::keyToInt(int key) {
    // Offset to get Qt::Key_A: 0
    if (key == Qt::Key_Escape) return 26;
    return key - Qt::Key_A;
}

/**
 * Executes a function from a KeyMap by calling the corresponding method
 * in EventHandler.
 * 
 * @param function  The function to execute
 */
void EventListener::executeFunction(std::string function) {
    // Check which function needs to be executed
    if (function == "movetocenter") eh->moveToCenter();
    if (function == "adddomain") eh->addDomain(mouse, true);
    if (function == "adddomainupdate") eh->addDomain(mouse, false);
    if (function == "addsource") eh->addSource();
    if (function == "addreceiver") eh->addReceiver();
    if (function == "cancelnewdomain") eh->cancelNewDomain();
    if (function == "selectstart") eh->selectStart(mouse);
    if (function == "selectupdate") eh->selectStop(mouse, false);
    if (function == "selectstop") eh->selectStop(mouse, true);
    if (function == "selectdomainstart") eh->selectStart(mouse);
    if (function == "selectdomainupdate") eh->selectDomainStop(mouse, false);
    if (function == "selectdomainstop") eh->selectDomainStop(mouse, true);
    if (function == "selectwallstart") eh->selectStart(mouse);
    if (function == "selectwallupdate") eh->selectWallStop(mouse, false);
    if (function == "selectwallstop") eh->selectWallStop(mouse, true);
    if (function == "changeabsorption") eh->changeAbsorptionDialog();
    if (function == "measure") eh->measure(mouse, true);
    if (function == "measureupdate") eh->measure(mouse, false);
    if (function == "removemeasure") eh->removeMeasure();
    if (function == "movestart") eh->moveStart();
    if (function == "moveupdate") eh->moveStop(mouse - oldmouse, false);
    if (function == "movestop") eh->moveStop(mouse - oldmouse, true);
    if (function == "movecoordinates") eh->moveCoordinates(mouse);
}