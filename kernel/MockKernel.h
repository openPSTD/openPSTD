//
// Created by michiel on 24-1-2016.
//

#ifndef OPENPSTD_MOCKKERNEL_H
#define OPENPSTD_MOCKKERNEL_H

#include "KernelInterface.h"

class MockKernel: public KernelInterface
{
private:
    std::shared_ptr<PSTDFileConfiguration> _conf;

    PSTD_FRAME_PTR CreateRandomFrame(int x, int y);

public:
    /**
     * Sets the configuration.
     */
    virtual void start_kernel(std::shared_ptr<PSTDFileConfiguration> config) override;

    /**
     * Runs the kernel. The callback has a single function that informs the rest of the
     * application of the progress of the kernel.
     * Must first be configured, else a PSTDKernelNotConfiguredException is thrown.
     */
    virtual void run(KernelCallback *callback) override;

    /**
     * Query the kernel for metadata about the simulation that is configured.
     */
    virtual SimulationMetadata get_metadata() override;
};


#endif //OPENPSTD_MOCKKERNEL_H
