//
// Created by michiel on 31-8-2015.
//

#ifndef OPENPSTD_SELECTDOMAINOPERATION_H
#define OPENPSTD_SELECTDOMAINOPERATION_H

#include "BaseOperation.h"

class SelectIndexedObjectOperation : public BaseOperation
{
private:
    SelectionType type;
    int index;
public:
    SelectIndexedObjectOperation(SelectionType type, int index);

    virtual void Run(const Reciever &reciever) override;
};

class SelectDomainOperation : public SelectIndexedObjectOperation
{
public:
    SelectDomainOperation(int selectDomainIndex);
};

class DeselectOperation : public SelectIndexedObjectOperation
{
public:
    DeselectOperation();
};

class SelectObjectOperation: public BaseOperation
{
private:
    QVector2D ScreenPosition;
public:
    SelectObjectOperation(QVector2D ScreenPosition);

    virtual void Run(const Reciever &reciever) override;
};


#endif //OPENPSTD_SELECTDOMAINOPERATION_H
