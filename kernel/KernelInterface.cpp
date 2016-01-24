//
// Created by michiel on 24-1-2016.
//

#include "KernelInterface.h"

const char* PSTDKernelNotConfiguredException::what() const noexcept
{
    return "Kernel is not yet configured";
}