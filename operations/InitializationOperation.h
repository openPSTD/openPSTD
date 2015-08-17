//
// Created by michiel on 17-8-2015.
//

#ifndef OPENPSTD_INITIALIZATIONOPERATION_H
#define OPENPSTD_INITIALIZATIONOPERATION_H

#include "BaseOperation.h"

class InitializationOperation: public BaseOperation
{
public:
    virtual void Run(const Reciever &reciever);

};


#endif //OPENPSTD_INITIALIZATIONOPERATION_H