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
// Date: 18-7-2015
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "Viewer2D.h"
#include <qopenglcontext.h>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <algorithm>
#include <math.h>
#include <string>
#include <memory>
#include "GridLayer.h"
#include "SceneLayer.h"
#include "IconLayer.h"
#include "InteractiveLayer.h"
#include <boost/lexical_cast.hpp>
#include "operations/ViewOperations.h"
#include "operations/LambdaOperation.h"
#include "mouse/MouseStrategy.h"

namespace OpenPSTD
{
    namespace GUI
    {

        void GLError(std::string name)
        {
            GLenum err = glGetError();
            if (err != GL_NO_ERROR)
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

        void DeleteNothing(void *ptr)
        {

        }

        void DeleteTexture(void *ptr)
        {
            if (ptr != nullptr)
                ((QOpenGLTexture *) ptr)->destroy();
        }

        Viewer2D::Viewer2D(QWidget *parent)
                : QOpenGLWidget(parent), layers()
        {
            this->layers.push_back(std::make_shared<GridLayer>());
            this->layers.push_back(std::make_shared<SceneLayer>());
            this->layers.push_back(std::make_shared<IconLayer>());
            this->layers.push_back(std::make_shared<InteractiveLayer>());
        }

        void Viewer2D::SetOperationRunner(std::weak_ptr<OperationRunner> operationRunner)
        {
            this->operationRunner = operationRunner;
        }


        void Viewer2D::paintGL()
        {
            if(this->loaded)
            {
                std::unique_ptr<QOpenGLFunctions, void (*)(void *)> f(QOpenGLContext::currentContext()->functions(),
                                                                      DeleteNothing);

                for (int i = 0; i < this->layers.size(); i++)
                {
                    this->layers[i]->PaintGL(this, f);
                    GLError("Viewer2D:: this->layers[" + boost::lexical_cast<std::string>(i) + "]->PaintGL");
                }
            }
        }

        void Viewer2D::initializeGL()
        {
            std::unique_ptr<QOpenGLFunctions, void (*)(void *)> f(QOpenGLContext::currentContext()->functions(),
                                                                  DeleteNothing);

            f->glClearColor(0, 0, 0, 1);

            f->glDisable(GL_CULL_FACE);

            for (int i = 0; i < this->layers.size(); i++)
            {
                this->layers[i]->InitializeGL(this, f);
                GLError("Viewer2D:: this->layers[" + boost::lexical_cast<std::string>(i) + "]->InitializeGL");
            }
        }

        void Viewer2D::resizeGL(int w, int h)
        {
            if (!this->operationRunner.expired())
            {
                this->operationRunner.lock()->RunOperation(std::make_shared<ChangeAspectMatrix>(w, h));
            }

            std::unique_ptr<QOpenGLFunctions, void (*)(void *)> f(QOpenGLContext::currentContext()->functions(),
                                                                  DeleteNothing);
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
            std::unique_ptr<QOpenGLFunctions, void (*)(void *)> f(QOpenGLContext::currentContext()->functions(),
                                                                  DeleteNothing);

            if (model->settings->IsChanged())
            {
                QColor clearColor = model->colorScheme->EditorBackgroundColor();
                f->glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
            }

            this->loaded = model->documentAccess->IsDocumentLoaded();
            if(this->loaded)
            {
                for (int i = 0; i < this->layers.size(); i++)
                {
                    this->layers[i]->UpdateScene(model, f);
                    GLError("Viewer2D:: this->layers[" + boost::lexical_cast<std::string>(i) + "]->UpdateScene");
                }
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
            if (!first.Active && !second.Active)
            {
                result.Active = false;
            }
            else if (!first.Active)
            {
                for (int i = 0; i < 2; i++)
                {
                    result.min[i] = second.min[i];
                    result.max[i] = second.max[i];
                }
            }
            else if (!second.Active)
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
            for (MinMaxValue v: list)
            {
                result = Combine(result, v);
            }
            return result;
        }

        void Viewer2D::mousePressEvent(QMouseEvent *event)
        {
            QPoint ScreenPos = event->pos();
            QVector2D pos = QVector2D(ScreenPos.x() / ((float) this->width()) * 2 - 1,
                                      -1 * (ScreenPos.y() / ((float) this->height()) * 2 - 1));

            std::shared_ptr<LambdaOperation> op = std::make_shared<LambdaOperation>([&](const Reciever &reciever) {
                reciever.model->mouseHandler->mousePressEvent(reciever.model, event, pos);
            });
            this->operationRunner.lock()->RunOperation(op);
        }

        void Viewer2D::mouseReleaseEvent(QMouseEvent *event)
        {
            QPoint ScreenPos = event->pos();
            QVector2D pos = QVector2D(ScreenPos.x() / ((float) this->width()) * 2 - 1,
                                      -1 * (ScreenPos.y() / ((float) this->height()) * 2 - 1));

            std::shared_ptr<LambdaOperation> op = std::make_shared<LambdaOperation>([&](const Reciever &reciever) {
                reciever.model->mouseHandler->mouseReleaseEvent(reciever.model, event, pos);
            });
            this->operationRunner.lock()->RunOperation(op);
        }

        void Viewer2D::mouseMoveEvent(QMouseEvent *event)
        {
            QPoint ScreenPos = event->pos();
            QVector2D pos = QVector2D(ScreenPos.x() / ((float) this->width()) * 2 - 1,
                                      -1 * (ScreenPos.y() / ((float) this->height()) * 2 - 1));

            std::shared_ptr<LambdaOperation> op = std::make_shared<LambdaOperation>([&](const Reciever &reciever) {
                reciever.model->mouseHandler->mouseMoveEvent(reciever.model, event, pos);
            });
            this->operationRunner.lock()->RunOperation(op);
        }

        void Viewer2D::wheelEvent(QWheelEvent *event)
        {
            QPoint ScreenPos = event->pos();
            QVector2D pos = QVector2D(ScreenPos.x() / ((float) this->width()) * 2 - 1,
                                      -1 * (ScreenPos.y() / ((float) this->height()) * 2 - 1));

            std::shared_ptr<LambdaOperation> op = std::make_shared<LambdaOperation>([&](const Reciever &reciever) {
                reciever.model->mouseHandler->wheelEvent(reciever.model, event, pos);
            });
            this->operationRunner.lock()->RunOperation(op);
        }

    }
}
