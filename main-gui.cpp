//
// Created by michiel on 18-7-2015.
//

#include <iostream>
#include <QtWidgets/qapplication.h>
#include "MainWindow.h"
#include "main-gui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}