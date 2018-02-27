#include "simulator.h"

Simulator::Simulator(Model* model, QAction* showoutput) {
    // Save reference to Model instance
    this->model = model;
    this->showoutput = showoutput;
    
    // Hide the output console by default
    shown = false;
    showoutput->setChecked(false);
    threadRunning = false;
}

Simulator::~Simulator() {
    // Stop the simulator thread if it is running
    if (threadRunning) {
        thread.join();
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
    
    // Show the output console
    shown = true;
    showoutput->setChecked(true);
    
    // Create a new file for this simulation
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
    //runcmd("../OpenPSTD-cli run -f test");
}

void Simulator::draw(QImage* pixels) {
    // Do nothing if the output console is not shown
    if (!shown) return;
    
    // Draw the background
    int w = 300;
    for (int x = pixels->width() - w; x < pixels->width(); x++) {
        for (int y = 0; y < pixels->height(); y++) {
            pixels->setPixel(x, y, qRgb(255, 255, 255));
        }
    }
    
    // Loop through all lines in the output vector
    int maxchars = 31;
    int line = 0;
    for (int i = 0; i < output.size(); i++) {
        int offset = 0;
        while (offset < output[i].length()) {
            std::string sub = output[i].substr(offset, maxchars);
            drawText(
                sub,
                pixels->width() - w,
                15 * line,
                12,
                qRgb(0, 0, 0),
                pixels
            );
            offset += maxchars;
            line++;
        }
    }
}

void Simulator::toggle() {
    shown = !shown;
    showoutput->setChecked(shown);
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
            result += buffer.data();
        }
    }
    
    // Output the commands output
    std::cout << "<" << result << std::endl;
    output.push_back("<" + result);
}

void Simulator::drawText(std::string text, int x, int y, int size, QRgb color, QImage* pixels) {
    QPainter p;
    p.begin(pixels);
    p.setPen(QPen(color));
    p.setFont(QFont("Monospace", size));
    p.drawText(x, y + size, QString(text.c_str()));
    p.end();
}