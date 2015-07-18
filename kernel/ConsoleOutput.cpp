//
// Created by michiel on 12-7-2015.
//

#include "ConsoleOutput.h"
#include <iostream>

void ConsoleOutput::Callback(CALLBACKSTATUS status, std::string message, int frame)
{
    if(status == CALLBACKSTATUS_ERROR)
    {
        std::cerr << "Error encountered while running OpenPSTD:" << std::endl;
        std::cerr << message << std::endl;
    }
    else if(status == CALLBACKSTATUS_STARTING)
    {
        std::cout << message << std::endl;
    }
    else if(status == CALLBACKSTATUS_RUNNING)
    {
        std::cout << message << std::endl;
    }
    else if(status == CALLBACKSTATUS_SUCCES)
    {
        std::cout << message << std::endl;
    }
}
