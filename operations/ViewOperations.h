//
// Created by michiel on 16-8-2015.
//

#ifndef OPENPSTD_VIEWOPERATIONS_H
#define OPENPSTD_VIEWOPERATIONS_H

#include "BaseOperation.h"
#include <QMatrix4x4>
#include <QVector2D>

class ChangeViewMatrix: public BaseOperation
{
private:
    QMatrix4x4 M;

public:
    ChangeViewMatrix(): M() {}
    ChangeViewMatrix(QMatrix4x4 m);

    virtual void Run(const Reciever &reciever);
};

class ModifyViewMatrix: public BaseOperation
{
protected:
    QMatrix4x4 M;

public:
    ModifyViewMatrix(): M() {}
    ModifyViewMatrix(QMatrix4x4 m);

    virtual void Run(const Reciever &reciever);
};

class ViewWholeScene: public BaseOperation
{
public:
    virtual void Run(const Reciever &reciever);
};

class TranslateScene: public ModifyViewMatrix
{
public:
    TranslateScene(QVector2D vec);
};

class ResizeScene: public ModifyViewMatrix
{
public:
    ResizeScene(float scale, QVector2D pos);
};

#endif //OPENPSTD_VIEWOPERATIONS_H



