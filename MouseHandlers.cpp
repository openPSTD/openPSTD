//
// Created by michiel on 17-8-2015.
//

#include "MouseHandlers.h"
#include "operations/ViewOperations.h"
#include <iostream>

void MouseStrategy::SetOperationRunner(std::shared_ptr<OperationRunner> operationRunner)
{
    this->operationRunner = operationRunner;
}

void MouseMoveSceneStrategy::mouseMoveEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos)
{
    QVector2D offset = pos - this->mousePos;
    this->mousePos = pos;

    auto buttons = mouseEvent->buttons();
    if(buttons & Qt::LeftButton)
    {
        std::shared_ptr<TranslateScene> op(new TranslateScene(offset));
        operationRunner->RunOperation(op);
    }
}

void MouseMoveSceneStrategy::wheelEvent(std::shared_ptr<Model> const &model, QWheelEvent *mouseEvent, QVector2D pos)
{
    float delta = mouseEvent->delta();
    float delta2 = powf(2, delta/120);

    std::shared_ptr<ResizeScene> op(new ResizeScene(delta2, pos));
    operationRunner->RunOperation(op);
}

void MouseMoveSceneStrategy::mousePressEvent(std::shared_ptr<Model> const &model, QMouseEvent *event, QVector2D pos)
{
    this->mousePos = pos;
}
