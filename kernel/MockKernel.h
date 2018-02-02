//
// Created by michiel on 24-1-2016.
//

#ifndef OPENPSTD_MOCKKERNEL_H
#define OPENPSTD_MOCKKERNEL_H

#include "KernelInterface.h"

namespace OpenPSTD
{
    namespace Kernel
    {
        /**
         * Fake kernel useful for testing the interaction between kernel and CLI or GUI
         */
        class MockKernel : public KernelInterface
        {
        private:
            std::shared_ptr<PSTDConfiguration> _conf;

            PSTD_FRAME_PTR CreateRandomFrame(int x, int y);

            PSTD_FRAME_PTR CreateHorizontalLines(int x, int y);

            PSTD_FRAME_PTR CreateVerticalLines(int x, int y);

            PSTD_FRAME_PTR CreateHorizontalGradient(int x, int y);

            PSTD_FRAME_PTR CreateVerticalGradient(int x, int y);

            PSTD_FRAME_PTR CreateVerticalGradientNeg(int x, int y);


        public:
            /**
             * Sets the configuration.
             */
            virtual void initialize_kernel(std::shared_ptr<PSTDConfiguration> config) override;

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

    }
}

#endif //OPENPSTD_MOCKKERNEL_H
