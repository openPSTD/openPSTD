#ifndef SIMULATOR2_H
#define SIMULATOR2_H

#include <QStatusBar>
#include <pthread.h>
#include <sys/stat.h>
#include "simulatoroutput.h"
#include "modelmanager.h"
#include "frame.h"

class Simulator2 : public QObject {
	Q_OBJECT
public:
	Simulator2(QWidget* parent, QStatusBar* statusbar);
	~Simulator2();
	
	void start();
	void stop();
	
	inline void draw(QImage* pixels) { so->draw(pixels); }
	inline void toggle() { so->toggle(); }
	
	inline void mousePress(int x, int y) { so->mousePress(x, y); }
private:
	// Reference variables
	QStatusBar* statusbar;
	SimulatorOutput* so;
	
	// Settings variables
	std::string kernel = "/home/jeroen/programming/openPSTD/GUI/build/OpenPSTD-cli";
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
	
	void exec(std::string cmd);
	void runSimulation(std::string cmd);
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