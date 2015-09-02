//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_KERNELFACADE_H
#define OPENPSTD_KERNELFACADE_H

#include <string>

/**
 * The configuration of the kernel.
 */
struct KernelConfiguration {
    bool multiThreaded;
    bool gpuAccelerated;
    bool writePlot;
    bool writeArray;
};

/**
 * The status of the kernel when the callback is called.
 */
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
    /**
     * This callback will be called with information how far the kernel is progressed.
     * Frame is only used with status == CALLBACKSTATUS_RUNNING, else it's -1.
     */
    virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) = 0;
};

/**
 * A facade before the kernel. This is a simlpe C++ api that can be used for calling the kernel.
 */
class KernelFacade
{
private:
    KernelConfiguration configuration;

    void OutputPythonHomeAndPath();

public:
    /**
     * Configures the kernel
     */
    void Configure(KernelConfiguration configuration);

    /**
     * Runs the kernel with a certain scene file. The callback has a single function that informs the rest of the
     * application how the progress is of the kernel.
     */
    void Run(std::string sceneFile, KernelFacadeCallback* callback);
};


#endif //OPENPSTD_KERNELFACADE_H
