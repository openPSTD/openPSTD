#include <QApplication>
#include "window.h"
#include "test/testgrid.h"
#include "test/testutility.h"
#include "test/testsource.h"
#include "test/testreceiver.h"

int main(int argc, char** argv) {
    // Create QApplication and Window instances
    int argcm = argc - 1;
    QApplication a(argcm, argv);
    Window window;
    
    // Check if "test" argument is provided
    if (argc == 2 && strcmp(argv[1], "test") == 0) {
        // Execute test cases
        int status = 0;
        
        TestGrid grid(&window);
        status |= QTest::qExec(&grid, argcm, argv);
        
        TestUtility utility(&window);
        status |= QTest::qExec(&utility, argcm, argv);
        
        TestSource source(&window);
        status |= QTest::qExec(&source, argcm, argv);
        
        TestReceiver receiver(&window);
        status |= QTest::qExec(&receiver, argcm, argv);
        
        return status;
    } else {
        // Run the regular application
        window.show();
        return a.exec();
    }
}