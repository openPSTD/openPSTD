#include "simulator.h"

/**
 * Constructor.
 * 
 * @param model  A reference to the model
 * @param showoutput  A reference to the showoutput action
 */
Simulator::Simulator(Model* model, QAction* showoutput) {
    // Save reference to Model instance
    this->model = model;
    this->showoutput = showoutput;
    
    // Initialize the state variables
    shown = false;
    showoutput->setChecked(false);
    threadRunning = false;
    numcomputed = 0;
    shownFrame = -1;
    numframes = 1000;
    playspeed = 0;
}

/**
 * Destructor.
 */
Simulator::~Simulator() {
    // Stop the simulator thread if it is running
    if (threadRunning) {
        thread.join();
    }
    
    // Delete all frames
    for (int i = 0; i < frames.size(); i++) {
        delete frames[i];
    }
}

/**
 * Starts the simulation.
 */
void Simulator::start() {
    // Stop the simulator thread if it is running
    if (threadRunning) {
        thread.join();
    }
    
    // Run the simulator in a new thread
    thread = std::thread(&Simulator::run, this);
    threadRunning = true;
}

/**
 * Multithreaded run method, called by Simulator::start.
 */
void Simulator::run() {
    // TMP: Setup the scene as expected by the fake kernel
    model->domains.clear();
    model->domains.push_back(Domain(0, 0, 10, -15));
    model->domains.push_back(Domain(10, -4, 30, -19));
    model->sources.clear();
    model->sources.push_back(Source(5, -4));
    model->receivers.clear();
    model->receivers.push_back(Receiver(6, -5));
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        model->domains[i].resetWalls();
    }
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        model->domains[i].mergeDomains(&model->domains, i);
    }
    numframes = 340;
    
    // Reset the output console
    output.clear();
    numcomputed = 0;
    shownFrame = -1;
    
    // Show the output console
    shown = true;
    showoutput->setChecked(true);
    
    /*// Create a new file for this simulation
    runcmd("../OpenPSTD-cli create test");
    
    // Add all domains
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        int x0 = model->domains[i].getX0();
        int x1 = model->domains[i].getX1();
        int y0 = model->domains[i].getY0();
        int y1 = model->domains[i].getY1();
        
        std::string cmd = "../OpenPSTD-cli edit -d [";
        cmd += std::to_string(x0) + ",";
        cmd += std::to_string(y0) + ",";
        cmd += std::to_string(x1-x0) + ",";
        cmd += std::to_string(y1-y0);
        cmd += "] -f test";
        runcmd(cmd);
    }
    
    // Add all sources
    for (unsigned int i = 0; i < model->sources.size(); i++) {
        int x = model->sources[i].getX();
        int y = model->sources[i].getY();
        
        std::string cmd = "../OpenPSTD-cli edit -p [";
        cmd += std::to_string(x) + ",";
        cmd += std::to_string(y);
        cmd += "] -f test";
        runcmd(cmd);
    }
    
    // Add all receivers
    for (unsigned int i = 0; i < model->receivers.size(); i++) {
        int x = model->receivers[i].getX();
        int y = model->receivers[i].getY();
        
        std::string cmd = "../OpenPSTD-cli edit -r [";
        cmd += std::to_string(x) + ",";
        cmd += std::to_string(y);
        cmd += "] -f test";
        runcmd(cmd);
    }
    
    // Run the simulation
    runcmd("../OpenPSTD-cli run -f test");*/
    runcmd("../fakekernel " + std::to_string(numframes));
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
        int px = model->receivers[0].getX() * model->zoom + model->offsetX;
        int py = model->receivers[0].getY() * model->zoom + model->offsetY;
        int x0 = model->domains[0].getX0();
        int x1 = model->domains[0].getX1();
        int y0 = model->domains[0].getY0();
        int y1 = model->domains[0].getY1();
        int minx = x0 * model->zoom + model->offsetX;
        int maxx = x1 * model->zoom + model->offsetX;
        int miny = y0 * model->zoom + model->offsetY;
        int maxy = y1 * model->zoom + model->offsetY;
        int fwidth = frames[0]->getWidth(0);
        int fheight = frames[0]->getHeight(0);
        int ix = (px - minx) * (fwidth-1) / (maxx - minx) + 1;
        int iy = (maxy - py) * (fheight-1) / (maxy - miny) + 1;
        
        for (int x = 0; x < pixels->width(); x++) {
            int i = x * numframes / pixels->width();
            int m = (x * numframes) % pixels->width();
            double t = (double) m / pixels->width();
            if (i >= numcomputed) break;
            double v0 = frames[i]->getSample(ix, iy, 0);
            double v1 = (m == 0 || i+1 >= numcomputed ? v0 : frames[i+1]->getSample(ix, iy, 0));
            double value = v0 * (1-t) + v1 * t;
            int y = pixels->height() - 10 - h/2 - value * scale;
            pixels->setPixel(x, y, qRgb(255, 255, 255));
        }
    }
    
    // Draw the buttons
    int x0 = pixels->width() / 2;
    int y = pixels->height() - h - 20;
    drawImage(x0 - 60, y, ":/new/prefix1/icons/output_home.png", pixels);
    drawImage(x0 - 40, y, ":/new/prefix1/icons/output_playreversed.png", pixels);
    drawImage(x0 - 20, y, ":/new/prefix1/icons/output_pause.png", pixels);
    drawImage(x0     , y, ":/new/prefix1/icons/output_play.png", pixels);
    drawImage(x0 + 20, y, ":/new/prefix1/icons/output_end.png", pixels);
    drawImage(x0 + 40, y, ":/new/prefix1/icons/output_sound.png", pixels);
    
    // Draw the resulting pressure on the scene
    for (unsigned int i = 0; i < model->domains.size(); i++) {
        if (frames.size() == 0) return;
        int fwidth = frames[0]->getWidth(i);
        int fheight = frames[0]->getHeight(i);
        
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
        for (int y = miny; y <= maxy; y++) {
            for (int x = minx; x <= maxx; x++) {
                // Do nothing if this pixel is not visible
                if (x < 0 || x >= pixels->width()) continue;
                if (y < 0 || y >= pixels->height()) continue;
                
                // Interpolate this pixel to the kernel grid
                int ix = (x - minx) * (fwidth-1) / (maxx - minx);
                int iy = (maxy - y) * (fheight-1) / (maxy - miny);
                
                // Get the pressure value at this position
                double pressure = frames[shownFrame]->getSample(ix, iy, i);
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
    if (x0-60 <= x && x < x0-40) button_home();
    if (x0-40 <= x && x < x0-20) button_playreversed();
    if (x0-20 <= x && x < x0   ) button_pause();
    if (x0    <= x && x < x0+20) button_play();
    if (x0+20 <= x && x < x0+40) button_end();
    if (x0+40 <= x && x < x0+60) button_sound();
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
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    int frameID = 0;
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            // Output the printed line
            std::string s = buffer.data();
            result += s;
            //std::cout << "-" << s;
            output.push_back("<" + result);
            
            // Save the new frame
            Frame* f = new Frame(frameID);
            frames.push_back(f);
            frameID++;
            
            // Update numcomputed
            numcomputed++;
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
    p.drawImage(QRect(x, y, 20, 20), QImage(filename.c_str()));
    p.drawRect(x, y, 20, 20);
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