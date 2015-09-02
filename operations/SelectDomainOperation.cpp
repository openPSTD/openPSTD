//
// Created by michiel on 31-8-2015.
//

#include "SelectDomainOperation.h"
#include <iostream>

SelectDomainOperation::SelectDomainOperation(int selectDomainIndex)
{
    this->selectDomainIndex = selectDomainIndex;
}

void SelectDomainOperation::Run(const Reciever &reciever)
{
    reciever.model->interactive->SelectedDomainIndex = this->selectDomainIndex;
    reciever.model->interactive->Change();
    std::cout << "Selected domain: " << reciever.model->interactive->SelectedDomainIndex << std::endl;
}