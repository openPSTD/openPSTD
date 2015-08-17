//
// Created by michiel on 17-8-2015.
//

#ifndef OPENPSTD_MOUSEHANDLERS_H
#define OPENPSTD_MOUSEHANDLERS_H

class MouseStrategy;
class MouseVoidStrategy;
class MouseMoveSceneStrategy;

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

    virtual void mousePressEvent(QMouseEvent *, QVector2D pos){};
    virtual void mouseReleaseEvent(QMouseEvent *mouseEvent, QVector2D pos){};
    virtual void mouseMoveEvent(QMouseEvent *mouseEvent, QVector2D pos){};
    virtual void wheelEvent(QWheelEvent *mouseEvent, QVector2D pos){};
};

class MouseVoidStrategy : public MouseStrategy
{
};

class MouseMoveSceneStrategy : public MouseStrategy
{
private:
    QVector2D mousePos;
public:
    virtual void mousePressEvent(QMouseEvent *, QVector2D pos);

    virtual void mouseMoveEvent(QMouseEvent *mouseEvent, QVector2D pos);

    virtual void wheelEvent(QWheelEvent *mouseEvent, QVector2D pos);
};

#endif //OPENPSTD_MOUSEHANDLERS_H


