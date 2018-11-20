#include <QApplication>
#include "window.h"

int main(int argc, char** argv) {
	// Create QApplication and Window instances
	QApplication a(argc, argv);
	Window window;
	
	// Run the regular application
	window.show();
	return a.exec();
}