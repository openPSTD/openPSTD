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
//      10-9-2015
//
// Authors:
//      michiel
//
//////////////////////////////////////////////////////////////////////////

#include "IconLayer.h"
#include "PstdAlgorithm.h"
namespace OpenPSTD
{
    namespace GUI
    {
        IconLayer::IconLayer() : lines(0)
        {

        }

        void IconLayer::InitializeGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            QColor color(0, 255, 255, 255);

            f->glGenBuffers(1, &this->LineBuffers);
            f->glGenBuffers(1, &this->ColorBuffer);

            std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Symbol.vert.glsl"));
            std::unique_ptr<std::string> geoFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Symbol.geo.glsl"));
            std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Symbol.frag.glsl"));

            program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
            program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
            program->addShaderFromSourceFile(QOpenGLShader::Geometry, QString::fromStdString(*geoFile));
            program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
            program->link();

            program->bind();
        }

        void IconLayer::PaintGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            program->bind();

            program->enableAttributeArray("a_position");
            f->glBindBuffer(GL_ARRAY_BUFFER, this->LineBuffers);
            f->glVertexAttribPointer((GLuint) program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
            GLError("a_position");

            program->enableAttributeArray("a_color");
            f->glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
            f->glVertexAttribPointer((GLuint) program->attributeLocation("a_color"), 4, GL_FLOAT, GL_FALSE, 0, 0);
            GLError("a_color");

            f->glLineWidth(1.0f);
            f->glDrawArrays(GL_POINTS, 0, lines);
            GLError("f->glDrawArrays");
            program->disableAttributeArray("a_position");
            program->disableAttributeArray("a_color");

        }

        void IconLayer::UpdateScene(std::shared_ptr<Model> const &m,
                                    std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            if (m->documentAccess->IsChanged())
            {
                QVector2D SizeLines(0.4, 0.4);
                QVector2D SizeSquare(0.2, 0.2);
                std::unique_ptr<std::vector<QVector2D>> positions(new std::vector<QVector2D>());
                std::unique_ptr<std::vector<QVector4D>> colors(new std::vector<QVector4D>());

                std::vector<QVector2D> speakers = GetSpeakers(m);
                for (int i = 0; i < speakers.size(); i++)
                {
                    positions->push_back(speakers[i]);
                    colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorSpeakerColor()));
                }

                std::vector<QVector2D> receivers = GetReceivers(m);
                for (int i = 0; i < receivers.size(); i++)
                {
                    positions->push_back(receivers[i]);
                    colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorReceiverColor()));
                }

                this->lines = speakers.size() + receivers.size();

                f->glBindBuffer(GL_ARRAY_BUFFER, this->LineBuffers);
                f->glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(QVector2D), positions->data(),
                                GL_DYNAMIC_DRAW);
                f->glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
                f->glBufferData(GL_ARRAY_BUFFER, colors->size() * sizeof(QVector4D), colors->data(), GL_DYNAMIC_DRAW);
            }

            if (m->view->IsChanged())
            {
                program->bind();
                program->setUniformValue("u_view", m->view->viewMatrix);
            }
        }

        MinMaxValue IconLayer::GetMinMax()
        {
            MinMaxValue result;
            result.Active = false;
            return result;
        }

        std::vector<QVector2D> IconLayer::GetSpeakers(std::shared_ptr<Model> const &m)
        {
            std::vector<QVector2D> result;
            auto conf = m->documentAccess->GetDocument()->GetSceneConf();

            for (int i = 0; i < conf->Speakers.size(); ++i)
            {
                result.push_back(conf->Speakers[i].toVector2D());
            }

            return result;
        }

        std::vector<QVector2D> IconLayer::GetReceivers(std::shared_ptr<Model> const &m)
        {
            std::vector<QVector2D> result;
            auto conf = m->documentAccess->GetDocument()->GetSceneConf();

            for (int i = 0; i < conf->Receivers.size(); ++i)
            {
                result.push_back(conf->Receivers[i].toVector2D());
            }
            return result;
        }
    }
}
