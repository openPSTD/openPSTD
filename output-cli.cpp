//
// Created by michiel on 24-1-2016.
//

#include "output-cli.h"
#include <iostream>
#include <boost/lexical_cast.hpp>

void CLIOutput::Callback(CALLBACKSTATUS status, std::string message, int frame)
{
    if (status == CALLBACKSTATUS::ERROR)
    {
        std::cerr << "Error encountered while running OpenPSTD:" << std::endl;
        std::cerr << message << std::endl;
    }
    else if (status == CALLBACKSTATUS::STARTING)
    {
        std::cout << message << std::endl;
    }
    else if (status == CALLBACKSTATUS::RUNNING)
    {
        std::cout << message << std::endl;
    }
    else if (status == CALLBACKSTATUS::FINISHED)
    {
        std::cout << message << std::endl;
    }
}

void CLIOutput::WriteFrame(int frame, std::string domain, PSTD_FRAME_PTR data)
{
    _file->SaveNextFrame(boost::lexical_cast<int>(domain), data);
}