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
// Date: 26-8-2015
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////


#include <boost/lexical_cast.hpp>
#include "SceneLayer.h"
#include "GUI/Edges.h"

namespace OpenPSTD
{
    namespace GUI
    {
        SceneLayer::SceneLayer() : lines(0), positionsBuffer(), valuesBuffer()
        {

        }

        void SceneLayer::InitializeGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            f->glGenBuffers(1, &this->positionsBuffer);
            f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);

            f->glGenBuffers(1, &this->valuesBuffer);
            f->glBindBuffer(GL_ARRAY_BUFFER, this->valuesBuffer);

            std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Scene2D.vert"));
            std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Scene2D.frag"));

            program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
            program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
            program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
            program->link();

            program->bind();

            program->setUniformValue("vmin", 0.0f);
            program->setUniformValue("vmax", 1.0f);
        }

        void SceneLayer::PaintGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            program->bind();

            program->enableAttributeArray("a_position");
            f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);
            f->glVertexAttribPointer((GLuint) program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);

            program->enableAttributeArray("a_value");
            f->glBindBuffer(GL_ARRAY_BUFFER, this->valuesBuffer);
            f->glVertexAttribPointer((GLuint) program->attributeLocation("a_value"), 1, GL_FLOAT, GL_FALSE, 0, 0);

            f->glLineWidth(5.0f);
            f->glDrawArrays(GL_LINES, 0, lines * 2);
            program->disableAttributeArray("a_position");
            program->disableAttributeArray("a_value");

            f->glBindTexture(GL_TEXTURE_1D, 0);
            f->glActiveTexture(GL_TEXTURE0);

            for(int i = 0; i < this->dimensions->size(); i++)
            {
                auto info = (*this->dimensions)[i];
                this->textRenderer->Draw(f, this->viewMatrix, info.position, 1.0f,
                                         info.text, QColor(0, 128, 128), info.vAlign, info.hAlign);
            }
        }

        void SceneLayer::UpdateScene(std::shared_ptr<Model> const &m,
                                     std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            program->bind();
            if (m->view->IsChanged())
            {
                this->viewMatrix = m->view->viewMatrix;
                program->setUniformValue("u_view", this->viewMatrix);
            }

            if (m->settings->IsChanged())
            {
                this->lineWidth = m->settings->visual.EdgeSize;
            }

            if (m->documentAccess->IsChanged())
            {
                this->dimensions = GetAllEdgeText(m);

                std::unique_ptr<std::vector<Edge>> edges = GetAllEdges(m);
                std::unique_ptr<std::vector<Edge>> edgesWithoutDuplicates = RemoveDuplicateEdges(std::move(edges));

                std::unique_ptr<std::vector<QVector2D>> positions(new std::vector<QVector2D>());
                std::unique_ptr<std::vector<float>> absorption(new std::vector<float>());
                std::unique_ptr<std::vector<bool>> localReacting(
                        new std::vector<bool>());//is not used yet, but for future use

                //reserve for speed improvement
                positions->reserve(edgesWithoutDuplicates->size() * 2);
                absorption->reserve(edgesWithoutDuplicates->size() * 2);
                localReacting->reserve(edgesWithoutDuplicates->size() * 2);

                for (int i = 0; i < edgesWithoutDuplicates->size(); i++)
                {
                    positions->push_back((*edgesWithoutDuplicates)[i].GetStart());
                    absorption->push_back((*edgesWithoutDuplicates)[i].GetAbsorption());
                    localReacting->push_back((*edgesWithoutDuplicates)[i].GetLocalReacting());

                    positions->push_back((*edgesWithoutDuplicates)[i].GetEnd());
                    absorption->push_back((*edgesWithoutDuplicates)[i].GetAbsorption());
                    localReacting->push_back((*edgesWithoutDuplicates)[i].GetLocalReacting());
                }

                this->lines = edgesWithoutDuplicates->size();

                f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);
                f->glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(QVector2D), positions->data(),
                                GL_DYNAMIC_DRAW);

                f->glBindBuffer(GL_ARRAY_BUFFER, this->valuesBuffer);
                f->glBufferData(GL_ARRAY_BUFFER, absorption->size() * sizeof(float), absorption->data(),
                                GL_DYNAMIC_DRAW);
            }
        }

        MinMaxValue SceneLayer::GetMinMax()
        {
            return MinMaxValue();
        }

        std::unique_ptr<std::vector<Edge>> SceneLayer::GetAllEdges(std::shared_ptr<Model> const &m)
        {
            auto conf = m->documentAccess->GetDocument()->GetSceneConf();
            std::unique_ptr<std::vector<Edge>> result(new std::vector<Edge>());

            for (int i = 0; i < conf->Domains.size(); i++)
            {
                QVector2D tl = conf->Domains[i].TopLeft;
                QVector2D size = conf->Domains[i].Size;
                QVector2D br = tl + size;

                float left = tl[0];
                float top = tl[1];

                float right = br[0];
                float bottom = br[1];

                result->push_back(Edge(QVector2D(left, top), QVector2D(left, bottom), conf->Domains[i].L.Absorption,
                                       conf->Domains[i].L.LR));
                result->push_back(Edge(QVector2D(right, top), QVector2D(right, bottom), conf->Domains[i].R.Absorption,
                                       conf->Domains[i].R.LR));

                result->push_back(Edge(QVector2D(left, bottom), QVector2D(right, bottom), conf->Domains[i].B.Absorption,
                                       conf->Domains[i].B.LR));
                result->push_back(Edge(QVector2D(left, top), QVector2D(right, top), conf->Domains[i].T.Absorption,
                                       conf->Domains[i].T.LR));
            }

            return std::move(result);
        };

        std::unique_ptr<std::vector<SceneLayer::DimensionText>> SceneLayer::GetAllEdgeText(std::shared_ptr<Model> const &m)
        {
            auto conf = m->documentAccess->GetDocument()->GetSceneConf();
            std::unique_ptr<std::vector<SceneLayer::DimensionText>> result(new std::vector<SceneLayer::DimensionText>());

            for (int i = 0; i < conf->Domains.size(); i++)
            {
                QVector2D tl = conf->Domains[i].TopLeft;
                QVector2D size = conf->Domains[i].Size;
                QVector2D br = tl + size;

                float left = tl[0];
                float top = tl[1];

                float right = br[0];
                float bottom = br[1];

                std::ostringstream ss;
                ss << std::setprecision(2);
                ss << fabs(bottom - top);
                std::string height = ss.str();

                ss.clear();
                ss.str("");
                ss << std::setprecision(2);
                ss << fabs(left - right);
                std::string width = ss.str();

                result->push_back(DimensionText(QVector2D(left, (bottom + top) / 2), height, TextHorizontalAlignment::RIGHT, TextVerticalAlignment::CENTER));
                result->push_back(DimensionText(QVector2D(right, (bottom + top) / 2), height, TextHorizontalAlignment::LEFT, TextVerticalAlignment::CENTER));
                result->push_back(DimensionText(QVector2D((left + right) / 2, top), width, TextHorizontalAlignment::CENTER, TextVerticalAlignment::BOTTOM));
                result->push_back(DimensionText(QVector2D((left + right) / 2, bottom), width, TextHorizontalAlignment::CENTER, TextVerticalAlignment::TOP));
            }

            return std::move(result);
        }


        std::unique_ptr<std::vector<Edge>> SceneLayer::RemoveDuplicateEdges(std::unique_ptr<std::vector<Edge>> edges)
        {
            std::unique_ptr<std::vector<Edge>> result(new std::vector<Edge>());

            for (int i = 0; i < edges->size(); i++)
            {
                std::vector<Edge> tmp = Edge::SubstractListFromEdge((*edges)[i], *edges, {i});
                result->insert(result->end(), tmp.begin(), tmp.end());
            }
            return std::move(result);
        }

    }
}