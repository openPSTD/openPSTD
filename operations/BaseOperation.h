//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_BASEOPERATION_H
#define OPENPSTD_BASEOPERATION_H

#include "../Model.h"

class OperationRunner;
class Reciever;
class BaseOperation;

class OperationRunner
{
public:
    virtual void RunOperation(BaseOperation operation) = 0;
};

class Reciever
{
public:
    Model* model;
};

class BaseOperation
{
    virtual void Run(const Reciever &reciever) = 0;
};


#endif //OPENPSTD_BASEOPERATION_H
