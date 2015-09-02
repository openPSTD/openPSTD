//
// Created by michiel on 18-7-2015.
//

#include "BaseOperation.h"


void LambdaOperation::Run(const Reciever &reciever)
{
    this->_func(reciever);
}

LambdaOperation::LambdaOperation(std::function<void(const Reciever &)> func): _func(func)
{
}
