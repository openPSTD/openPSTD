//
// Created by michiel on 24-8-2015.
//

#include "MouseOperations.h"

ChangeMouseHandlerOperations::ChangeMouseHandlerOperations(std::unique_ptr<MouseStrategy> mouseHandler) :
        mouseHandler(std::move(mouseHandler))
{

}

void ChangeMouseHandlerOperations::Run(const Reciever &reciever)
{
    reciever.model->mouseHandler = std::move(this->mouseHandler);
    reciever.model->mouseHandler->SetOperationRunner(reciever.operationRunner);
}

