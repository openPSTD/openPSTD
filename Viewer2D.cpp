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
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/\>.    //
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
// Created by michiel on 18-7-2015.
//

#include "Viewer2D.h"
#include <qopenglcontext.h>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <algorithm>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <string>
#include <memory>
#include "GridLayer.h"
#include "SceneLayer.h"
#include "IconLayer.h"
#include "InteractiveLayer.h"
#include <boost/lexical_cast.hpp>
#include "operations/ViewOperations.h"


void GLError(std::string name)
{
    GLenum err = glGetError();
    if(err != GL_NO_ERROR)
    {
        std::cout << "------------------------------------------------------" << std::endl;
        std::cout << "=" << name << std::endl;

        while (err != GL_NO_ERROR)
        {
            std::cout << err << ": " << gluErrorString(err) << std::endl;
            err = glGetError();
        }
    }
}

void DeleteNothing(void * ptr)
{

}

void DeleteTexture(void * ptr)
{
    if(ptr != nullptr)
        ((QOpenGLTexture*)ptr)->destroy();
}

Viewer2D::Viewer2D(QWidget *parent)
    : QOpenGLWidget(parent), layers()
{
    std::cout << "create layers" << std::endl;
    //this->layers.push_back(std::shared_ptr<GridLayer>(new GridLayer()));
    this->layers.push_back(std::shared_ptr<SceneLayer>(new SceneLayer()));
    this->layers.push_back(std::shared_ptr<IconLayer>(new IconLayer()));
    this->layers.push_back(std::shared_ptr<InteractiveLayer>(new InteractiveLayer()));
}

void Viewer2D::SetOperationRunner(std::shared_ptr<OperationRunner> operationRunner)
{
    this->operationRunner = operationRunner;
}


void Viewer2D::paintGL()
{
    auto del = [](int * p) { std::cout << "Deleting x, value is : " << *p; };
    std::unique_ptr<QOpenGLFunctions, void(*)(void*)> f(QOpenGLContext::currentContext()->functions(), DeleteNothing);

    for(int i = 0; i < this->layers.size(); i++)
    {
        this->layers[i]->PaintGL(this, f);
        GLError("Viewer2D:: this->layers[" + boost::lexical_cast<std::string>(i) + "]->PaintGL");
    }
}

void Viewer2D::initializeGL()
{
    std::unique_ptr<QOpenGLFunctions, void(*)(void*)> f(QOpenGLContext::currentContext()->functions(), DeleteNothing);

    f->glClearColor(0, 0, 0, 1);

    f->glDisable(GL_CULL_FACE);

    for(int i = 0; i < this->layers.size(); i++)
    {
        this->layers[i]->InitializeGL(this, f);
        GLError("Viewer2D:: this->layers[" + boost::lexical_cast<std::string>(i) + "]->InitializeGL");
    }
}

void Viewer2D::resizeGL(int w, int h)
{
    if(this->operationRunner)
    {
        this->operationRunner->RunOperation(std::shared_ptr<ChangeAspectMatrix>(new ChangeAspectMatrix(w, h)));
    }

    std::unique_ptr<QOpenGLFunctions, void(*)(void*)> f(QOpenGLContext::currentContext()->functions(), DeleteNothing);
    f->glViewport(0, 0, w, h);
}

QSize Viewer2D::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize Viewer2D::sizeHint() const
{
    return QSize(400, 400);
}

void Viewer2D::UpdateFromModel(std::shared_ptr<Model> const &model)
{
    std::unique_ptr<QOpenGLFunctions, void(*)(void*)> f(QOpenGLContext::currentContext()->functions(), DeleteNothing);

    if(model->settings->IsChanged())
    {
        QColor clearColor = model->settings->visual.colorScheme->EditorBackgroundColor();
        f->glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    }

    for(int i = 0; i < this->layers.size(); i++)
    {
        this->layers[i]->UpdateScene(model, f);
        GLError("Viewer2D:: this->layers[" + boost::lexical_cast<std::string>(i) + "]->UpdateScene");
    }
}

MinMaxValue::MinMaxValue()
{

}

MinMaxValue::MinMaxValue(QVector2D min, QVector2D max)
{
    this->min = min;
    this->max = max;
    this->Active = true;
}

MinMaxValue MinMaxValue::Combine(MinMaxValue first, MinMaxValue second)
{
    using namespace boost::numeric::ublas;
    MinMaxValue result;
    if(!first.Active && !second.Active)
    {
        result.Active = false;
    }
    else if(!first.Active)
    {
        for (int i = 0; i < 2; i++)
        {
            result.min[i] = second.min[i];
            result.max[i] = second.max[i];
        }
    }
    else if(!second.Active)
    {
        for (int i = 0; i < 2; i++)
        {
            result.min[i] = first.min[i];
            result.max[i] = first.max[i];
        }
    }
    else
    {
        for (int i = 0; i < 2; i++)
        {
            result.min[i] = std::min(first.min[i], second.min[i]);
            result.max[i] = std::max(first.max[i], second.max[i]);
        }
    }
    return result;
}

MinMaxValue MinMaxValue::CombineList(std::vector<MinMaxValue> list)
{
    MinMaxValue result = list.back();
    list.pop_back();
    for(MinMaxValue v: list)
    {
        result = Combine(result, v);
    }
    return result;
}

void Viewer2D::mousePressEvent(QMouseEvent *event)
{
    QPoint ScreenPos = event->pos();
    QVector2D pos = QVector2D(ScreenPos.x()/((float)this->width())*2-1, -1*(ScreenPos.y()/((float)this->height())*2-1));

    std::shared_ptr<LambdaOperation> op(new LambdaOperation([&](const Reciever &reciever){
        reciever.model->mouseHandler->mousePressEvent(reciever.model, event, pos);
    }));
    this->operationRunner->RunOperation(op);
}

void Viewer2D::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint ScreenPos = event->pos();
    QVector2D pos = QVector2D(ScreenPos.x()/((float)this->width())*2-1, -1*(ScreenPos.y()/((float)this->height())*2-1));

    std::shared_ptr<LambdaOperation> op(new LambdaOperation([&](const Reciever &reciever){
        reciever.model->mouseHandler->mouseReleaseEvent(reciever.model, event, pos);
    }));
    this->operationRunner->RunOperation(op);
}

void Viewer2D::mouseMoveEvent(QMouseEvent *event)
{
    QPoint ScreenPos = event->pos();
    QVector2D pos = QVector2D(ScreenPos.x()/((float)this->width())*2-1, -1*(ScreenPos.y()/((float)this->height())*2-1));

    std::shared_ptr<LambdaOperation> op(new LambdaOperation([&](const Reciever &reciever){
        reciever.model->mouseHandler->mouseMoveEvent(reciever.model, event, pos);
    }));
    this->operationRunner->RunOperation(op);
}

void Viewer2D::wheelEvent(QWheelEvent *event)
{
    QPoint ScreenPos = event->pos();
    QVector2D pos = QVector2D(ScreenPos.x()/((float)this->width())*2-1, -1*(ScreenPos.y()/((float)this->height())*2-1));

    std::shared_ptr<LambdaOperation> op(new LambdaOperation([&](const Reciever &reciever){
        reciever.model->mouseHandler->wheelEvent(reciever.model, event, pos);
    }));
    this->operationRunner->RunOperation(op);
}
