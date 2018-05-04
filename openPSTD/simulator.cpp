#include "simulator.h"

/**
 * Constructor.
 * 
 * @param model  A reference to the model
 * @param showoutput  A reference to the showoutput action
 */
Simulator::Simulator(Model* model, QAction* showoutput, QStatusBar* statusbar) {
    // Save reference to Model instance
    this->model = model;
    this->showoutput = showoutput;
    this->statusbar = statusbar;
    
    // Initialize the state variables
    shown = false;
    showoutput->setChecked(false);
    threadRunning = false;
    numcomputed = 0;
    shownFrame = -1;
    numframes = 1000;
    playspeed = 0;
    setReceiver(0);
}

/**
 * Destructor.
 */
Simulator::~Simulator() {
    // Stop the simulator thread if it is running
    if (threadRunning) {
        pthread_cancel(thread);
    }
}

/**
 * Starts the simulation.
 */
void Simulator::start() {
    // Reset the output console
    output.clear();
    numcomputed = 0;
    shownFrame = -1;
    
    // Show the output console
    shown = true;
    showoutput->setChecked(true);
    
    // Stop the simulator thread if it is running
    if (threadRunning) {
        pthread_cancel(thread);
    }
    
    // Run the simulator in a new thread
    threadID = pthread_create(&thread, NULL, &Simulator::runstatic, this);
    threadRunning = true;
}

/**
 * Multithreaded run method, called by Simulator::start.
 */
void Simulator::run() {
    // Show the pressure at the first receiver initially
    setReceiver(0);
    
    // Create a new file for this simulation
    runcmd(kernel + " create test");
    
    // Remove the two default domains, source, and receiver
    runcmd(kernel + " edit -D 0 -f test");
    runcmd(kernel + " edit -D 0 -f test");
    runcmd(kernel + " edit -R 0 -f test");
    runcmd(kernel + " edit -P 0 -f test");
    
    // Add all domains
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        // Get the coordinates of the domain
        int x0 = model->domains[i].getX0();
        int x1 = model->domains[i].getX1();
        int y0 = model->domains[i].getY0();
        int y1 = model->domains[i].getY1();
        
        // Add the domain
        std::string cmd = kernel + " edit -d [";
        cmd += std::to_string(x0) + ",";
        cmd += std::to_string(-y1) + ",";
        cmd += std::to_string(x1-x0) + ",";
        cmd += std::to_string(y1-y0);
        cmd += "] -f test";
        runcmd(cmd);
        
        // Get the absorption coefficients of the four walls
        double at = model->domains[i].getAbsorption(TOP);
        double ab = model->domains[i].getAbsorption(BOTTOM);
        double al = model->domains[i].getAbsorption(LEFT);
        double ar = model->domains[i].getAbsorption(RIGHT);
        
        // Set the absorption coefficients of the four walls
        runcmd(kernel + " edit -a \"(" + std::to_string(i) + ",t," + std::to_string(at) + ")\" -f test");
        runcmd(kernel + " edit -a \"(" + std::to_string(i) + ",b," + std::to_string(ab) + ")\" -f test");
        runcmd(kernel + " edit -a \"(" + std::to_string(i) + ",l," + std::to_string(al) + ")\" -f test");
        runcmd(kernel + " edit -a \"(" + std::to_string(i) + ",r," + std::to_string(ar) + ")\" -f test");
        
        // Get the edge local reacting values of the four walls
        std::string et = (model->domains[i].getEdgeLocalReacting(TOP) ? "true" : "false");
        std::string eb = (model->domains[i].getEdgeLocalReacting(BOTTOM) ? "true" : "false");
        std::string el = (model->domains[i].getEdgeLocalReacting(LEFT) ? "true" : "false");
        std::string er = (model->domains[i].getEdgeLocalReacting(RIGHT) ? "true" : "false");
        
        // Set the edge local reacting values of the four walls
        runcmd(kernel + " edit -l \"(" + std::to_string(i) + ",t," + et + ")\" -f test");
        runcmd(kernel + " edit -l \"(" + std::to_string(i) + ",b," + eb + ")\" -f test");
        runcmd(kernel + " edit -l \"(" + std::to_string(i) + ",l," + el + ")\" -f test");
        runcmd(kernel + " edit -l \"(" + std::to_string(i) + ",r," + er + ")\" -f test");
    }
    
    // Add all sources
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        int x = model->sources[i].getX();
        int y = model->sources[i].getY();
        
        std::string cmd = kernel + " edit -p [";
        cmd += std::to_string(-y) + ","; // TODO: Coordinates are swapped because of kernel issue
        cmd += std::to_string(x);
        cmd += "] -f test";
        runcmd(cmd);
    }
    
    // Add all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        int x = model->receivers[i].getX();
        int y = model->receivers[i].getY();
        
        std::string cmd = kernel + " edit -r [";
        cmd += std::to_string(x) + ",";
        cmd += std::to_string(-y);
        cmd += "] -f test";
        runcmd(cmd);
    }
    
    // Update the PSTD settings
    Settings* settings = Settings::getInstance();
    runcmd(kernel + " edit --grid-spacing " + std::to_string(settings->pstdGridSize) + " -f test");
    runcmd(kernel + " edit --window-size " + std::to_string(settings->windowSize) + " -f test");
    runcmd(kernel + " edit --render-time " + std::to_string(settings->renderTime) + " -f test");
    runcmd(kernel + " edit --pml-cells " + std::to_string(settings->pmlcells) + " -f test");
    runcmd(kernel + " edit --attenuation-of-pml-cells " + std::to_string(settings->attenuationpmlcells) + " -f test");
    runcmd(kernel + " edit --density-of-air " + std::to_string(settings->airDensity) + " -f test");
    runcmd(kernel + " edit --sound-speed " + std::to_string(settings->soundSpeed) + " -f test");
    
    // Reset the output directory
    system("rm -r testdata/");
    system("mkdir testdata");
    
    // Run the simulation
    runcmd(kernel + " run -f test");
}

/**
 * Public drawing method: Draws all simulation related pixels.
 * 
 * @param pixels  A reference to the QImage to draw on
 */
void Simulator::draw(QImage* pixels) {
    // Do nothing if the output console is not shown
    if (!shown) return;
    
    // Update width
    mutex.lock();
    width = pixels->width();
    
    // Update shownFrame
    shownFrame += playspeed;
    if (shownFrame < 0) {
        shownFrame = 0;
        playspeed = 0;
    }
    if (shownFrame >= numcomputed) {
        shownFrame = numcomputed - 1;
        playspeed = 0;
    }
    
    // Draw the resulting pressure on the scene
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        if (frames.size() == 0) {
            mutex.unlock();
            return;
        }
        int fwidth = frames[0].getWidth(i);
        int fheight = frames[0].getHeight(i);
        
        // Get the corner coordinates of this domain
        int x0 = model->domains[i].getX0();
        int x1 = model->domains[i].getX1();
        int y0 = model->domains[i].getY0();
        int y1 = model->domains[i].getY1();
        
        // Convert to screen coordinates
        int minx = x0 * model->zoom + model->offsetX;
        int maxx = x1 * model->zoom + model->offsetX;
        int miny = y0 * model->zoom + model->offsetY;
        int maxy = y1 * model->zoom + model->offsetY;
        
        // Loop through all pixels in the domain
        for (int y = miny; y < maxy; y++) {
            for (int x = minx; x < maxx; x++) {
                // Do nothing if this pixel is not visible
                if (x < 0 || x >= pixels->width()) continue;
                if (y < 0 || y >= pixels->height()) continue;
                
                /*// Interpolate this pixel to the kernel grid
                double tx = (double) (x - minx) / (maxx - minx);
                double ty = (double) (maxy - y) / (maxy - miny);
                int ix0 = (int) (tx * (fwidth-1));
                int ix1 = ix0 + 1;
                int iy0 = (int) (ty * (fheight-1));
                int iy1 = iy0 + 1;
                double ttx = tx - ix0/(fwidth-1);
                double tty = ty - iy0/(fheight-1);
                
                double p00 = frames[shownFrame]->getSample(ix0, iy0, i);
                double p01 = frames[shownFrame]->getSample(ix0, iy1, i);
                double p10 = frames[shownFrame]->getSample(ix1, iy0, i);
                double p11 = frames[shownFrame]->getSample(ix1, iy1, i);
                
                double px0 = (1-ttx)*p00 + ttx*p01;
                double px1 = (1-ttx)*p10 + ttx*p11;
                double pressure = (1-tty)*px0 + tty*px1;*/
                
                double xk = (double) (x-minx) * (fwidth-1) / (maxx-minx-3);
                double yk = (double) (maxy-y) * (fheight-1) / (maxy-miny-3);
                int xk0 = (int) xk;
                int xk1 = xk0 + 1;
                int yk0 = (int) yk;
                int yk1 = yk0 + 1;
                double tx = xk - xk0;
                double ty = yk - yk0;
                double p00 = frames[shownFrame].getSample(xk0, yk1, i);
                double p10 = frames[shownFrame].getSample(xk1, yk1, i);
                double p01 = frames[shownFrame].getSample(xk0, yk0, i);
                double p11 = frames[shownFrame].getSample(xk1, yk0, i);
                double pt0 = (1-tx)*p00 + tx*p01;
                double pt1 = (1-tx)*p01 + tx*p11;
                double pressure = p00;
                
                // Get the pressure value at this position
                if (pressure < 0) pressure = 0;
                QRgb color = qRgb(pressure * 255 * brightness, 0, 0);
                
                // Draw the pressure on the scene
                pixels->setPixel(
                    x,
                    y,
                    color
                );
            }
        }
    }
    
    // Draw the background
    int h = 150;
    for (int x = 0; x < pixels->width(); x++) {
        int frame = numframes * x / pixels->width();
        for (int y = pixels->height() - h; y < pixels->height() - 20; y++) {
            if (numcomputed >= frame) {
                pixels->setPixel(x, y, qRgb(115, 115, 115));
            } else {
                pixels->setPixel(x, y, qRgb(70, 70, 70));
            }
        }
        for (int y = pixels->height() - 20; y < pixels->height(); y++) {
            int i = y - pixels->height();
            pixels->setPixel(x, y, qRgb(
                156 - 3*i,
                156 - 3*i,
                156 - 3*i
            ));
        }
    }
    
    // Draw the background grid
    int df = 50;
    for (int i = 0; i < numframes; i += df) {
        int x = i * pixels->width() / numframes;
        for (int y = pixels->height() - h; y < pixels->height() - 20; y++) {
            pixels->setPixel(x, y, qRgb(0, 0, 0));
        }
        
        int offset = 0;
        if (x < 1000) offset = -14;
        if (x < 100) offset = -10;
        if (x < 10) offset = 0;
        drawText(
            std::to_string(i),
            x + offset,
            pixels->height() - 16,
            12,
            qRgb(0, 0, 0),
            pixels
        );
    }
    
    // Draw a line at the shown frame
    if (shownFrame != -1) {
        int x = shownFrame * width / numframes;
        for (int y = pixels->height() - h; y < pixels->height() - 20; y++) {
            pixels->setPixel(x, y, qRgb(0, 255, 0));
        }
    }
    
    // Draw the id of the shown frame
    if (shownFrame != -1) {
        drawText(
            std::to_string(shownFrame),
            5,
            pixels->height() - 40,
            16,
            qRgb(0, 0, 0),
            pixels
        );
    }
    
    // Draw the samples of all frames
    if (frames.size() > 0) {
        int px = model->receivers[receiverID].getX() * model->zoom + model->offsetX;
        int py = model->receivers[receiverID].getY() * model->zoom + model->offsetY;
        int x0 = model->domains[receiverDID].getX0();
        int x1 = model->domains[receiverDID].getX1();
        int y0 = model->domains[receiverDID].getY0();
        int y1 = model->domains[receiverDID].getY1();
        int minx = x0 * model->zoom + model->offsetX;
        int maxx = x1 * model->zoom + model->offsetX;
        int miny = y0 * model->zoom + model->offsetY;
        int maxy = y1 * model->zoom + model->offsetY;
        int fwidth = frames[0].getWidth(0);
        int fheight = frames[0].getHeight(0);
        int ix = (px - minx) * (fwidth-1) / (maxx - minx) + 1;
        int iy = (maxy - py) * (fheight-1) / (maxy - miny) + 1;
        
        for (int x = 0; x < pixels->width(); x++) {
            int i = x * numframes / pixels->width();
            int m = (x * numframes) % pixels->width();
            double t = (double) m / pixels->width();
            if (i >= numcomputed) break;
            double v0 = frames[i].getSample(ix, iy, receiverDID);
            double v1 = (m == 0 || i+1 >= numcomputed ? v0 : frames[i+1].getSample(ix, iy, receiverDID));
            double value = v0 * (1-t) + v1 * t;
            int y = pixels->height() - 10 - h/2 - value * scale;
            pixels->setPixel(x, y, qRgb(255, 255, 255));
        }
    }
    
    // Draw the buttons
    int x0 = pixels->width() / 2;
    int y = pixels->height() - h - 32;
    drawImage(x0 - 96, y, ":/new/prefix1/icons/output_home.png", pixels);
    drawImage(x0 - 64, y, ":/new/prefix1/icons/output_playreversed.png", pixels);
    drawImage(x0 - 32, y, ":/new/prefix1/icons/output_pause.png", pixels);
    drawImage(x0     , y, ":/new/prefix1/icons/output_play.png", pixels);
    drawImage(x0 + 32, y, ":/new/prefix1/icons/output_end.png", pixels);
    drawImage(x0 + 64, y, ":/new/prefix1/icons/output_sound.png", pixels);
    mutex.unlock();
}

/**
 * Toggles the visibility of the simulator output
 */
void Simulator::toggle() {
    // Update the state variables
    shown = !shown;
    
    // Update the showoutput action
    showoutput->setChecked(shown);
}

/**
 * Sets the currently drawn frame ID from an x-coordinate
 * at which was clicked / dragged.
 * 
 * @param x  The x-coordinate of the mouse
 */
void Simulator::showFrame(int x) {
    // Compute the frame id from the x-coordinate
    int frame = numframes * x / width;
    
    // Ensure that 0 <= frame < numcomputed <= numframes
    if (frame < 0) frame = 0;
    if (frame >= numcomputed) frame = numcomputed - 1;
    if (frame >= numframes) frame = numframes - 1;
    
    // Update the state variables
    shownFrame = frame;
}

/**
 * Callback for when one of the buttons of the simulator
 * is clicked.
 * 
 * @param x  The x-coordinate at which was clicked.
 */
void Simulator::pressButton(int x) {
    // Compute which button was clicked, and delegate
    int x0 = width / 2;
    if (x0-96 <= x && x < x0-64) button_home();
    if (x0-64 <= x && x < x0-32) button_playreversed();
    if (x0-32 <= x && x < x0   ) button_pause();
    if (x0    <= x && x < x0+32) button_play();
    if (x0+32 <= x && x < x0+64) button_end();
    if (x0+64 <= x && x < x0+96) button_sound();
}

/**
 * Sets the ID of the receiver of which to display
 * the pressure in the pressure graph.
 * 
 * @param i  The receiver ID
 */
void Simulator::setReceiver(int i) {
    // Save the receiver ID
    receiverID = i;
    
    // Get the position of the receiver
    if (i >= model->receivers.size()) return;
    int rx = model->receivers[i].getX();
    int ry = model->receivers[i].getY();
    
    // Loop through all domains
    for (int j = 0; j < model->domains.size(); j++) {
        // Get the position of this domain
        int dx0 = model->domains[j].getX0();
        int dx1 = model->domains[j].getX1();
        int dy0 = model->domains[j].getY0();
        int dy1 = model->domains[j].getY1();
        
        // Check if the receiver is inside this domain
        bool x = dx0 <= rx && rx <= dx1;
        bool y = dy0 <= ry && ry <= dy1;
        if (x && y) {
            // Save the receiver domain ID
            receiverDID = j;
            break;
        }
    }
}

/**
 * Runs the kernel, and handles its output.
 * 
 * @param cmd  The command to run
 */
void Simulator::runcmd(std::string cmd) {
    // Output the executed command
    std::cout << ">" << cmd << std::endl;
    output.push_back(">" + cmd);
    
    // Execute the command and read its output
    //std::array<char, 128> buffer;
    char* buffer = new char[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    int frameID = 0;
    while (!feof(pipe.get())) {
        if (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
            // Output the printed line
            std::string s = buffer;
            for (char c : s) {
                if (c == '\n') {
                    // Full line read, only handle "Finished frame" lines
                    std::cout << result << std::endl;
                    if (result.substr(0, 16) != "Finished frame: ") {
                        result = "";
                        continue;
                    }
                    
                    // Frame finished, load the new frame
                    mutex.lock();
                    frames.push_back(Frame(frameID, model->domains.size()));
                    frameID++;
                    numcomputed++;
                    mutex.unlock();
                    
                    // Reset result for the next line
                    result = "";
                    continue;
                }
                result += c;
            }
        }
    }
}

/**
 * Private drawing method: Draws a string to the given pixels array.
 * 
 * @param text  The text to draw
 * @param x  The x position to draw at
 * @param y  The y position to draw at
 * @param size  The font size to draw with
 * @param color  The color to draw in
 * @param pixels  A reference to the pixels array to draw on
 */
void Simulator::drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels) {
    QPainter p;
    p.begin(pixels);
    p.setPen(QPen(color));
    p.setFont(QFont("Monospace", size));
    p.drawText(x, y + size, QString(text.c_str()));
    p.end();
}

/**
 * Private drawing method: Draws an image to the given pixels array.
 * 
 * @param x  The x position to draw at
 * @param y  The y position to draw at
 * @param filename  The filename of the imagefile to draw
 * @param pixels  A reference to the pixels array to draw on
 */
void Simulator::drawImage(int x, int y, std::string filename, QImage* pixels) {
    QPainter p;
    p.begin(pixels);
    p.drawImage(QRect(x, y, 32, 32), QImage(filename.c_str()));
    p.drawRect(x, y, 32, 32);
    p.end();
}

/**
 * Callback method for when the home button is pressed.
 */
void Simulator::button_home() {
    // Show the first frame (or no frame if none are computed)
    if (numcomputed == 0) {
        shownFrame = -1;
    } else {
        shownFrame = 0;
    }
}

/**
 * Callback method for when the play reversed button is pressed.
 */
void Simulator::button_playreversed() {
    // Update the state variables
    playspeed = -1;
}

/**
 * Callback method for when the pause button is pressed.
 */
void Simulator::button_pause() {
    // Update the state variables
    playspeed = 0;
}

/**
 * Callback method for when the play button is pressed.
 */
void Simulator::button_play() {
    // Update the state variables
    playspeed = 1;
}

/**
 * Callback method for when the end button is pressed.
 */
void Simulator::button_end() {
    // Show the last computed frame, or no frame if none are computed
    if (numcomputed == 0) {
        shownFrame = -1;
    } else {
        shownFrame = numcomputed - 1;
    }
}

/**
 * Callback method for when the sound button is pressed.
 */
void Simulator::button_sound() {}