//
// Created by michiel on 16-8-2015.
//

#include "ViewOperations.h"

ChangeViewMatrix::ChangeViewMatrix(QMatrix4x4 m): M(m)
{

}

void ChangeViewMatrix::Run(const Reciever &reciever)
{
    reciever.model->view->value = this->M;
    reciever.model->view->Change();
}

ModifyViewMatrix::ModifyViewMatrix(QMatrix4x4 m): M(m)
{

}

void ModifyViewMatrix::Run(const Reciever &reciever)
{
    reciever.model->view->value = this->M * reciever.model->view->value;
    reciever.model->view->Change();
}

TranslateScene::TranslateScene(QVector2D vector): ModifyViewMatrix()
{
    this->M.translate(vector.toVector3D());
}

ResizeScene::ResizeScene(float scale, QVector2D pos): ModifyViewMatrix()
{
    this->M.translate(pos.toVector3D());
    this->M.scale(scale);
    this->M.translate(-pos.toVector3D());
}

void ViewWholeScene::Run(const Reciever &reciever)
{
    //todo fix this command
}
