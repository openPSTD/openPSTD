//
// Created by michiel on 12-7-2015.
//

#ifndef OPENPSTD_CONSOLEOUTPUT_H
#define OPENPSTD_CONSOLEOUTPUT_H

#include "KernelFacade.h"

class ConsoleOutput: public KernelFacadeCallback
{
    virtual void Callback(CALLBACKSTATUS status, std::string message, int frame) override;
};

#endif //OPENPSTD_CONSOLEOUTPUT_H
