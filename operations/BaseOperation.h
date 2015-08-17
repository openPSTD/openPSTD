//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_BASEOPERATION_H
#define OPENPSTD_BASEOPERATION_H

class OperationRunner;
class Reciever;
class BaseOperation;

#include "../Model.h"
#include <memory>

class OperationRunner
{
public:
    virtual void RunOperation(std::shared_ptr<BaseOperation> operation) = 0;
};

class Reciever
{
public:
    std::shared_ptr<Model> model;
    std::shared_ptr<OperationRunner> operationRunner;
};

class BaseOperation
{
public:
    virtual void Run(const Reciever &reciever) = 0;
};

class LambdaOperation: public BaseOperation
{
private:
    std::function<void (const Reciever &)> _func;

public:
    LambdaOperation(std::function<void (const Reciever &)> func);
    virtual void Run(const Reciever &reciever) override;
};


#endif //OPENPSTD_BASEOPERATION_H
