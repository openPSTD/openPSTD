//
// Created by michiel on 16-8-2015.
//

#ifndef OPENPSTD_VIEWOPERATIONS_H
#define OPENPSTD_VIEWOPERATIONS_H

#include "BaseOperation.h"
#include <QMatrix4x4>
#include <QVector2D>

class UpdateViewMatrix: public BaseOperation
{
public:
    virtual void Run(const Reciever &reciever);
};

class ChangeAspectMatrix: public BaseOperation
{
private:
    QMatrix4x4 M;

public:
    ChangeAspectMatrix(): M() {}
    ChangeAspectMatrix(QMatrix4x4 m);
    ChangeAspectMatrix(float w, float h);

    virtual void Run(const Reciever &reciever);
};

class ChangeWorldMatrix: public BaseOperation
{
private:
    QMatrix4x4 M;

public:
    ChangeWorldMatrix(): M() {}
    ChangeWorldMatrix(QMatrix4x4 m);

    virtual void Run(const Reciever &reciever);
};



class ModifyWorldMatrix: public BaseOperation
{
protected:
    QMatrix4x4 M;

public:
    ModifyWorldMatrix(): M() {}
    ModifyWorldMatrix(QMatrix4x4 m);

    virtual void Run(const Reciever &reciever);
};

class ViewWholeScene: public BaseOperation
{
public:
    virtual void Run(const Reciever &reciever);
};

class TranslateScene: public ModifyWorldMatrix
{
public:
    TranslateScene(QVector2D vec);
};

class ResizeScene: public ModifyWorldMatrix
{
public:
    ResizeScene(float scale, QVector2D pos);
};

#endif //OPENPSTD_VIEWOPERATIONS_H



