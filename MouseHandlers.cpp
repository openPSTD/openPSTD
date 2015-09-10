//
// Created by michiel on 17-8-2015.
//

#include "MouseHandlers.h"
#include "operations/ViewOperations.h"
#include "operations/EditOperations.h"
#include "operations/SelectDomainOperation.h"
#include <iostream>
#include "Snapping.h"
#include "PstdAlgorithm.h"

void MouseStrategy::SetOperationRunner(std::shared_ptr<OperationRunner> operationRunner)
{
    this->operationRunner = operationRunner;
}

void MouseSelectStrategy::mouseReleaseEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos)
{
    std::shared_ptr<SelectObjectOperation> op(new SelectObjectOperation(pos));
    operationRunner->RunOperation(op);
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


void MouseCreateDomainStrategy::mousePressEvent(std::shared_ptr<Model> const &model, QMouseEvent *, QVector2D pos)
{
    QVector2D start = (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D();

    start = Snapping::Snap(model, start);

    model->interactive->CreateDomain.start = start;
    model->interactive->CreateDomain.currentEnd = start;
    model->interactive->CreateDomain.visible = true;
    model->interactive->Change();
}

void MouseCreateDomainStrategy::mouseMoveEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos)
{
    QVector2D end = (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D();

    end = Snapping::Snap(model, end);

    model->interactive->CreateDomain.currentEnd = end;
    model->interactive->Change();
}

void MouseCreateDomainStrategy::mouseReleaseEvent(std::shared_ptr<Model> const &model, QMouseEvent *mouseEvent, QVector2D pos)
{
    QVector2D end = (model->view->viewMatrix.inverted() * pos.toVector3D()).toVector2D();

    end = Snapping::Snap(model, end);

    model->interactive->CreateDomain.currentEnd = end;

    model->interactive->CreateDomain.visible = false;
    model->interactive->Change();
    std::shared_ptr<CreateDomainOperation> op(new CreateDomainOperation(model->interactive->CreateDomain.start,
                                                                        model->interactive->CreateDomain.currentEnd));
    this->operationRunner->RunOperation(op);
}

