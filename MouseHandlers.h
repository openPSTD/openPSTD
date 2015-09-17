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
// Created by michiel on 17-8-2015.
//

#ifndef OPENPSTD_MOUSEHANDLERS_H
#define OPENPSTD_MOUSEHANDLERS_H

class MouseStrategy;
class MouseVoidStrategy;
class MouseMoveSceneStrategy;
class MouseCreateDomainStrategy;

#include <memory>
#include "operations/BaseOperation.h"
#include <QVector2D>
#include <QtGui/qevent.h>

class MouseStrategy
{
protected:
    std::shared_ptr<OperationRunner> operationRunner;

public:
    void SetOperationRunner(std::shared_ptr<OperationRunner> operationRunner);

    virtual void mousePressEvent(std::shared_ptr<Model> const &model, QMouseEvent *, QVector2D pos){};
    virtual void mouseReleaseEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos){};
    virtual void mouseMoveEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos){};
    virtual void wheelEvent(std::shared_ptr<Model> const &model, QWheelEvent *mouseEvent, QVector2D pos){};
};

class MouseVoidStrategy : public MouseStrategy
{
};

class MouseSelectStrategy : public MouseStrategy
{
public:
    virtual void mouseReleaseEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos) override;
};

class MouseMoveSceneStrategy : public MouseStrategy
{
private:
    QVector2D mousePos;
public:
    virtual void mousePressEvent(std::shared_ptr<Model> const &model, QMouseEvent *, QVector2D pos) override;

    virtual void mouseMoveEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos) override;

    virtual void wheelEvent(std::shared_ptr<Model> const &model, QWheelEvent *mouseEvent, QVector2D pos) override;
};

class MouseCreateDomainStrategy : public MouseStrategy
{
private:
    QVector2D _start;
    QVector2D _end;
public:
    virtual void mousePressEvent(std::shared_ptr<Model> const &model, QMouseEvent *, QVector2D pos) override;

    virtual void mouseMoveEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos) override;

    virtual void mouseReleaseEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos) override;
};

#endif //OPENPSTD_MOUSEHANDLERS_H


