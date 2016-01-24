//
// Created by michiel on 24-1-2016.
//

#ifndef OPENPSTD_KERNELINTERFACE_H
#define OPENPSTD_KERNELINTERFACE_H

#include <string>
#include "PSTDFile.h"
#include "GeneralTypes.h"

/**
 * The status of the kernel when the callback is called.
 */
enum class CALLBACKSTATUS
{
    ERROR,
    STARTING,
    RUNNING,
    FINISHED,
};

class KernelCallback
{
public:
    /**
     * This callback will be called with information how far the kernel is progressed.
     * @param status: CALLBACKSTATUS enum, either one of starting/running/finishing/error.
     * @param message: Message to pass to callback handler
     * @param frame: either positive integer corresponding to time step of data or -1 when kernel is not running.
     */
    virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) = 0;

    /**
     * Return pressure data of scene to callback handler.
     * @param frame: Positive integer corresponding to time step of data.
     * @param domain: an identifier that identifies the domain
     * @param data: 1D row-major vector of pressure data.
     */
    virtual void WriteFrame(int frame, std::string domain, PSTD_FRAME_PTR data) = 0;
};

class SimulationMetadata
{
public:
    /**
     * The discretization of the domains, in order they were passed to the kernel.
     * A vector in which domain n is represented by a vector at the nth position.
     * In the "inner" vectors, v[0],v[1],v[2] correspond to size x,y,z.
     */
    std::vector<std::vector<int>> DomainMetadata;
    int Framecount;
};

/**
 * The kernel API. Contains one method for initialization and one for running the simulation.
 */
class KernelInterface
{
public:
    /**
     * Sets the configuration.
     */
    virtual void Configure(std::shared_ptr<PSTDFileConfiguration> config) = 0;

    /**
     * Runs the kernel. The callback has a single function that informs the rest of the
     * application of the progress of the kernel.
     * Must first be configured, else a PSTDKernelNotConfiguredException is thrown.
     */
    virtual void run(KernelCallback *callback) = 0;

    /**
     * Query the kernel for metadata about the simulation that is configured.
     */
    virtual SimulationMetadata GetSimulationMetadata() = 0;
};

class PSTDKernelNotConfiguredException : public std::exception
{
public:
    const char* what() const noexcept override;
};


#endif //OPENPSTD_KERNELINTERFACE_H
