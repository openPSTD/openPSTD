#include "simulator.h"

Simulator::Simulator(Model* model, QAction* showoutput) {
    // Save reference to Model instance
    this->model = model;
    this->showoutput = showoutput;
    
    // Hide the output console by default
    shown = false;
    showoutput->setChecked(false);
    threadRunning = false;
    numcomputed = 0;
    shownFrame = -1;
    numframes = 1000;
}

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

void Simulator::start() {
    // Stop the simulator thread if it is running
    if (threadRunning) {
        thread.join();
    }
    
    // Run the simulator in a new thread
    thread = std::thread(&Simulator::run, this);
    threadRunning = true;
}

void Simulator::run() {
    // Reset the output console
    output.clear();
    numcomputed = 0;
    shownFrame = -1;
    
    // Show the output console
    shown = true;
    showoutput->setChecked(true);
    
    numframes = 1000;
    
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

void Simulator::draw(QImage* pixels) {
    // Do nothing if the output console is not shown
    if (!shown) return;
    
    // Update width
    width = pixels->width();
    
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
    for (int x = 0; x < pixels->width(); x++) {
        int i = x * numframes / pixels->width();
        if (i >= numcomputed) break;
        int value = frames[i]->getSample(0, 0);
        int y = pixels->height() - 10 - h/2 + value;
        pixels->setPixel(x, y, qRgb(255, 255, 255));
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
}

void Simulator::toggle() {
    shown = !shown;
    showoutput->setChecked(shown);
}

bool Simulator::isShown() {
    return shown;
}

void Simulator::showFrame(int x) {
    // Compute the frame id from the x-coordinate
    int frame = numframes * x / width;
    
    if (frame < 0) frame = 0;
    if (frame > numcomputed) frame = numcomputed;
    if (frame >= numframes) frame = numframes - 1;
    shownFrame = frame;
}

void Simulator::pressButton(int x) {
    int x0 = width / 2;
    if (x0-60 <= x && x < x0-40) button_home();
    if (x0-40 <= x && x < x0-20) button_playreversed();
    if (x0-20 <= x && x < x0   ) button_pause();
    if (x0    <= x && x < x0+20) button_play();
    if (x0+20 <= x && x < x0+40) button_end();
    if (x0+40 <= x && x < x0+60) button_sound();
}

void Simulator::runcmd(std::string cmd) {
    // Output the executed command
    std::cout << ">" << cmd << std::endl;
    output.push_back(">" + cmd);
    
    // Execute the command and read its output
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            // Output the printed line
            std::string s = buffer.data();
            result += s;
            std::cout << "-" << s;
            output.push_back("<" + result);
            
            // Update numcomputed
            numcomputed++;
            
            // Save the new frame
            Frame* f = new Frame(s);
            frames.push_back(f);
        }
    }
}

void Simulator::drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels) {
    QPainter p;
    p.begin(pixels);
    p.setPen(QPen(color));
    p.setFont(QFont("Monospace", size));
    p.drawText(x, y + size, QString(text.c_str()));
    p.end();
}

void Simulator::drawImage(int x, int y, std::string filename, QImage* pixels) {
    QPainter p;
    p.begin(pixels);
    p.drawImage(QRect(x, y, 20, 20), QImage(filename.c_str()));
    p.drawRect(x, y, 20, 20);
    p.end();
}

void Simulator::button_home() {
    std::cout << "home" << std::endl;
}

void Simulator::button_playreversed() {
    std::cout << "play reversed" << std::endl;
}

void Simulator::button_pause() {
    std::cout << "pause" << std::endl;
}

void Simulator::button_play() {
    std::cout << "play" << std::endl;
}

void Simulator::button_end() {
    std::cout << "end" << std::endl;
}

void Simulator::button_sound() {
    std::cout << "sound" << std::endl;
}