//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_KERNELFACADE_H
#define OPENPSTD_KERNELFACADE_H

#include <string>

struct KernelConfiguration {
    bool multiThreaded;
    bool gpuAccelerated;
    bool writePlot;
    bool writeArray;
};

enum CALLBACKSTATUS
{
    CALLBACKSTATUS_ERROR,
    CALLBACKSTATUS_STARTING,
    CALLBACKSTATUS_RUNNING,
    CALLBACKSTATUS_SUCCES,
};

class KernelFacadeCallback
{
public:
    virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) = 0;
};

class KernelFacade
{
private:
    KernelConfiguration configuration;

    void OutputPythonHomeAndPath();

public:
    void Configure(KernelConfiguration configuration);

    void Run(std::string sceneFile, KernelFacadeCallback* callback);
};


#endif //OPENPSTD_KERNELFACADE_H
