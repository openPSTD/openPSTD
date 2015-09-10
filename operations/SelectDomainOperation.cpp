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
    reciever.model->interactive->Selection.Type = SELECTION_DOMAIN;
    reciever.model->interactive->Selection.SelectedIndex = this->selectDomainIndex;
    reciever.model->interactive->Change();
}

void DeselectDomainOperation::Run(const Reciever &reciever)
{
    reciever.model->interactive->Selection.Type = SELECTION_NONE;
    reciever.model->interactive->Selection.SelectedIndex = -1;
    reciever.model->interactive->Change();
}