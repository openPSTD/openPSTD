#include <QApplication>
#include "window.h"
#include "testmaintoolbar.h"

int main(int argc, char** argv) {
    // Create QApplication and Window instances
    int argcm = argc - 1;
    QApplication a(argcm, argv);
    Window window;
    
    // Check if "test" argument is provided
    if (argc == 2 && strcmp(argv[1], "test") == 0) {
        // Execute test cases
        int status = 0;
        
        TestMainToolbar mainToolbar(&window);
        status |= QTest::qExec(&mainToolbar, argc - 1, argv);
        
        return status;
    } else {
        // Run the regular application
        window.show();
        return a.exec();
    }
}