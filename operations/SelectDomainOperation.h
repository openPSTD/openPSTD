//
// Created by michiel on 31-8-2015.
//

#ifndef OPENPSTD_SELECTDOMAINOPERATION_H
#define OPENPSTD_SELECTDOMAINOPERATION_H

#include "BaseOperation.h"

class SelectDomainOperation : public BaseOperation
{
private:
    int selectDomainIndex;
public:
    SelectDomainOperation(int selectDomainIndex);

    virtual void Run(const Reciever &reciever) override;
};

class DeselectDomainOperation : public BaseOperation
{
public:
    virtual void Run(const Reciever &reciever) override;
};


#endif //OPENPSTD_SELECTDOMAINOPERATION_H
