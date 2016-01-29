//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date:
//
//
// Authors:
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

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



