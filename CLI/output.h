//
// Created by michiel on 24-1-2016.
//

#ifndef OPENPSTD_OUTPUT_CLI_H
#define OPENPSTD_OUTPUT_CLI_H

#include <shared/PSTDFile.h>
namespace OpenPSTD
{
    namespace CLI
    {
        class CLIOutput : public KernelCallback
        {
        private:
            std::shared_ptr<Shared::PSTDFile> _file;
        public:
            CLIOutput(std::shared_ptr<Shared::PSTDFile> file) : _file(file)
            { };

            virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) override;

            virtual void WriteFrame(int frame, int domain, PSTD_FRAME_PTR data) override;

            virtual void WriteSample(int startSample, int receiver, std::vector<float> data) override;
        };
    }
}
#endif //OPENPSTD_OUTPUT_CLI_H
