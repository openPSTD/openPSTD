//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_MAIN_GUI_H
#define OPENPSTD_MAIN_GUI_H

#include <memory>
#include "operations/BaseOperation.h"

int main(int argc, char *argv[]);

class Controller: public OperationRunner
{
public:
    Controller();

    void SetArguments(int argc, char *argv[]);
    int RunApplication();
    void SetOperationRunner(std::shared_ptr<OperationRunner> runner);

    virtual void RunOperation(std::shared_ptr<BaseOperation> operation);

private:
    bool runningOp;
    std::shared_ptr<OperationRunner> operationRunner;
    std::shared_ptr<Model> model;
    std::unique_ptr<QApplication> a;
    std::unique_ptr<MainWindow> w;

    int argc;
    char **argv;
};

#endif //OPENPSTD_MAIN_GUI_H
