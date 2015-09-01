//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_MAINWINDOW_H
#define OPENPSTD_MAINWINDOW_H

#include <QMainWindow>
#include <ui_MainWindow.h>
#include <memory>
#include "operations/BaseOperation.h"
#include "MouseHandlers.h"

class MainWindow: public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(std::shared_ptr<OperationRunner> operationRunner, QWidget *parent = 0);

        void UpdateFromModel(std::shared_ptr<Model> const &model);

    private:
        std::unique_ptr<Ui_MainWindow> ui;
        std::shared_ptr<OperationRunner> operationRunner;
        std::vector<QAction*> MouseHandlersActions;

        void ChangeMouseHandler(QAction *action, std::unique_ptr<MouseStrategy> mouseHandler);
        void EditSelectedDomain();

    public slots:
        void New();
        void Open();
        void Save();


};


#endif //OPENPSTD_MAINWINDOW_H
