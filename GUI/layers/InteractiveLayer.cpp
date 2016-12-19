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
//      30-8-2015
//
// Authors:
//      michiel
//
//////////////////////////////////////////////////////////////////////////

#include "InteractiveLayer.h"
#include "GUI/PstdAlgorithm.h"
namespace OpenPSTD
{
    namespace GUI
    {
        InteractiveLayer::InteractiveLayer() : addDomainVisible(false), newDomainBuffer(false)
        {

        }

        void InteractiveLayer::InitializeGL(QObject *context,
                                            std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            QColor color(1.0f, 1.0f, 1.0f, 1.0f);

            f->glGenBuffers(1, &this->newDomainBuffer);
            f->glGenBuffers(1, &this->selectionBuffer);

            std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Interactive.vert"));
            std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Interactive.frag"));

            program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
            program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
            program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
            program->link();

            program->bind();

            program->setUniformValue("u_color", color);
        }

        void InteractiveLayer::PaintGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            if (this->addDomainVisible)
            {
                program->bind();

                program->setUniformValue("u_color", newDomainColor);

                program->enableAttributeArray("a_position");
                f->glBindBuffer(GL_ARRAY_BUFFER, this->newDomainBuffer);
                f->glVertexAttribPointer((GLuint) program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0,
                                         0);

                f->glLineWidth(5.0f);
                f->glDrawArrays(GL_LINES, 0, 4 * 2);
                program->disableAttributeArray("a_position");
            }
            if (this->selectionVisible)
            {
                program->bind();

                program->setUniformValue("u_color", selectionColor);

                program->enableAttributeArray("a_position");
                f->glBindBuffer(GL_ARRAY_BUFFER, this->selectionBuffer);
                f->glVertexAttribPointer((GLuint) program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0,
                                         0);

                f->glLineWidth(1.0f);
                f->glDrawArrays(GL_LINES, 0, 4 * 2);
                program->disableAttributeArray("a_position");
            }
        }

        void InteractiveLayer::UpdateScene(std::shared_ptr<Model> const &m,
                                           std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            if (m->interactive->IsChanged() || m->documentAccess->IsChanged())
            {
                this->addDomainVisible = m->interactive->CreateDomain.visible;

                if (this->addDomainVisible)
                {
                    std::unique_ptr<std::vector<float>> positions(new std::vector<float>());

                    AddSquareBuffer(positions, m->interactive->CreateDomain.start,
                                    m->interactive->CreateDomain.currentEnd - m->interactive->CreateDomain.start);

                    f->glBindBuffer(GL_ARRAY_BUFFER, this->newDomainBuffer);
                    f->glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(float), positions->data(),
                                    GL_DYNAMIC_DRAW);
                }

                this->selectionVisible = (m->interactive->Selection.Type != SELECTION_NONE);

                if (this->selectionVisible)
                {
                    auto conf = m->documentAccess->GetDocument()->GetSceneConf();
                    int i = m->interactive->Selection.SelectedIndex;

                    std::unique_ptr<std::vector<float>> positions(new std::vector<float>());

                    if (m->interactive->Selection.Type == SELECTION_DOMAIN)
                    {
                        AddSquareBuffer(positions, conf->Domains[i].TopLeft, conf->Domains[i].Size);
                    }
                    else if (m->interactive->Selection.Type == SELECTION_RECEIVER)
                    {
                        QVector2D SizeLines(0.4, 0.4);
                        AddSquareBuffer(positions, conf->Receivers[i].toVector2D() - SizeLines / 2, SizeLines);
                    }
                    else if (m->interactive->Selection.Type == SELECTION_SPEAKER)
                    {
                        QVector2D SizeLines(0.4, 0.4);
                        AddSquareBuffer(positions, conf->Speakers[i].toVector2D() - SizeLines / 2, SizeLines);
                    }

                    f->glBindBuffer(GL_ARRAY_BUFFER, this->selectionBuffer);
                    f->glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(float), positions->data(),
                                    GL_DYNAMIC_DRAW);
                }
            }

            if (m->settings->IsChanged())
            {
                this->newDomainColor = m->colorScheme->EditorAddDomainColor();
                this->selectionColor = m->colorScheme->EditorSelectionColor();

            }

            if (m->view->IsChanged())
            {
                program->bind();
                program->setUniformValue("u_view", m->view->viewMatrix);
            }
        }

        MinMaxValue InteractiveLayer::GetMinMax()
        {
            MinMaxValue result;
            result.Active = false;
            return result;
        }
    }
}