//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_MAINWINDOW_H
#define OPENPSTD_MAINWINDOW_H

#include <ui_MainWindow.h>

class MainWindow: public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private:
        Ui_MainWindow *ui;
};


#endif //OPENPSTD_MAINWINDOW_H
