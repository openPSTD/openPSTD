#include <QApplication>
#include "window.h"
/*#include "test/testmaintoolbar.h"
#include "test/testmenuscene.h"
#include "test/testmenuview.h"
#include "test/testmodel.h"
#include "test/testadddomain.h"
#include "test/testaddsource.h"
#include "test/testaddreceiver.h"
#include "test/testundoredo.h"
#include "test/testrobustness.h"*/

int main(int argc, char** argv) {
	// Create QApplication and Window instances
	int argcm = argc - 1;
	QApplication a(argcm, argv);
	Window window;
	
	/*// Check if "test" argument is provided
	if (argc == 2 && strcmp(argv[1], "test") == 0) {
		// Execute test cases
		int status = 0;
		
		TestMainToolbar mainToolbar(&window);
		status |= QTest::qExec(&mainToolbar, argcm, argv);
		
		TestMenuScene menuScene(&window);
		status |= QTest::qExec(&menuScene, argcm, argv);
		
		TestMenuView menuView(&window);
		status |= QTest::qExec(&menuView, argcm, argv);
		
		TestModel model(&window);
		status |= QTest::qExec(&model, argcm, argv);
		
		TestAddDomain addDomain(&window);
		status |= QTest::qExec(&addDomain, argcm, argv);
		
		TestAddSource addSource(&window);
		status |= QTest::qExec(&addSource, argcm, argv);
		
		TestAddReceiver addReceiver(&window);
		status |= QTest::qExec(&addReceiver, argcm, argv);
		
		TestUndoRedo undoRedo(&window);
		status |= QTest::qExec(&undoRedo, argcm, argv);
		
		return status;
	} else if (argc == 3 && strcmp(argv[1], "testrobustness") == 0) {
		int res;
		if (strcmp(argv[2], "log") == 0) {
			res = TestRobustness::testLog(&window);
		} else {
			res = TestRobustness::test(std::atoi(argv[2]), &window);
		}
		return res;
	} else {*/
		// Run the regular application
		window.show();
		return a.exec();
	//}
}