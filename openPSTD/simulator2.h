#ifndef SIMULATOR2_H
#define SIMULATOR2_H

#include <QStatusBar>
#include <QMessageBox>
#include <pthread.h>
#include <sys/stat.h>
#include "simulatoroutput.h"
#include "modelmanager.h"
#include "frame.h"
#include "sidebar.h"

class Simulator2 : public QObject {
	Q_OBJECT
public:
	Simulator2(QWidget* parent, QStatusBar* statusbar, Sidebar* sidebar);
	~Simulator2();
	
	void start();
	void stop();
    void removeSimulation();
	
	inline void draw(QImage* pixels) { so->draw(pixels); }
	inline void toggle() { so->toggle(); }
	
	inline void mousePress(int x, int y) { so->mousePress(x, y); }
    inline void mouseDrag(int x, int y) { so->mouseDrag(x, y); }
    
    inline std::vector<Frame> getFrames() { return so->getFrames(); }
private:
	// Reference variables
	QStatusBar* statusbar;
	SimulatorOutput* so;
    Sidebar* sidebar;
    
	const std::string OS = "linux";
	
	// Settings variables
	std::string kernel = (OS == "linux" ? "./OpenPSTD-cli" : "OpenPSTD-cli.exe");
	std::string rmoutputdircmd = (OS == "linux" ? "[ -d testdata ] && rm -r testdata" : "rem testdata");
	std::string mkoutputdircmd = "mkdir testdata";
	std::string filename = "test"; // Filename for the scene file (for kernel)
	std::string frameFinishString = "Finished frame: ";
	std::string simulationFinishString = "Succesfully finished simulation";
	
	// State variables
	int threadID;
	pthread_t thread;
	bool threadRunning;
	std::vector<Frame> frames;
	std::string statusbarText;
	
	// Run simulator method
	static void* run(void* args);
	
	// Scene validation methods
	bool sceneValid();
	bool domainsConnected();
	bool domainsOverlap();
	bool sourcesInDomain();
	bool receiversInDomain();
	bool domainMinimumSize();
	
	int getDomainOfPoint(QPoint pos);
	void deselectDomainsWallsSources();
	
	void exec(std::string cmd);
	void runSimulation(std::string cmd);
	
	void showErrorPopup(std::string message);
signals:
	void updateText(QString text);
	void centerScene();
public slots:
	void setText(QString text) {
		this->statusbar->showMessage(text);
		this->statusbar->update();
	}
};

#endif