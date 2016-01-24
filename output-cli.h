//
// Created by michiel on 24-1-2016.
//

#ifndef OPENPSTD_OUTPUT_CLI_H
#define OPENPSTD_OUTPUT_CLI_H

#include "PSTDFile.h"
#include "Kernel/KernelInterface.h"

class CLIOutput: public KernelCallback
{
private:
    std::shared_ptr<PSTDFile> _file;
public:
    CLIOutput(std::shared_ptr<PSTDFile> file): _file(file) { };

    virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) override;
    virtual void WriteFrame(int frame, std::string domain, PSTD_FRAME_PTR data) override;
};

#endif //OPENPSTD_OUTPUT_CLI_H
