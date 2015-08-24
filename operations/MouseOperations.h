//
// Created by michiel on 24-8-2015.
//

#ifndef OPENPSTD_MOUSEOPERATIONS_H
#define OPENPSTD_MOUSEOPERATIONS_H

class ChangeMouseHandlerOperations;

#include "BaseOperation.h"
#include "../MouseHandlers.h"

class ChangeMouseHandlerOperations: public BaseOperation
{
private:
    std::unique_ptr<MouseStrategy> mouseHandler;

public:
    ChangeMouseHandlerOperations(std::unique_ptr<MouseStrategy> mouseHandler);

    virtual void Run(const Reciever &reciever) override;
};


#endif //OPENPSTD_MOUSEOPERATIONS_H
