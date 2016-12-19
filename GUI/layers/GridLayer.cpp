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
//      26-8-2015
//
// Authors:
//      michiel
//
//////////////////////////////////////////////////////////////////////////

#include "GridLayer.h"

namespace OpenPSTD
{
    namespace GUI
    {
        GridLayer::GridLayer() : positionsBuffer(), lines(0)
        {

        }

        void GridLayer::InitializeGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            QColor color(255, 255, 255, 255);

            gridSpacing = 0.2f;

            f->glGenBuffers(1, &this->positionsBuffer);
            f->glGenBuffers(1, &this->originPositionsBuffer);

            std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Grid.vert"));
            std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Grid.frag"));
            std::unique_ptr<std::string> geometricFile = std::unique_ptr<std::string>(new std::string(":/GPU/Line.geom"));

            program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
            program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
            program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
            program->addShaderFromSourceFile(QOpenGLShader::Geometry, QString::fromStdString(*geometricFile));
            program->link();

            program->bind();

            program->setUniformValue("u_color", color);
        }

        void GridLayer::PaintGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            program->bind();
            program->enableAttributeArray("a_position");

            f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);
            GLError("GridLayer f->glBindBuffer");
            f->glVertexAttribPointer((GLuint) program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
            GLError("GridLayer f->glVertexAttribPointer");
            program->setUniformValue("thickness", 0.01f);
            GLError("GridLayer f->glLineWidth");
            f->glDrawArrays(GL_LINES, 0, lines * 2);
            GLError("GridLayer f->glDrawArrays");

            f->glBindBuffer(GL_ARRAY_BUFFER, this->originPositionsBuffer);
            GLError("GridLayer f->glBindBuffer");
            f->glVertexAttribPointer((GLuint) program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
            GLError("GridLayer f->glVertexAttribPointer");
            program->setUniformValue("thickness", 0.03f);
            GLError("GridLayer f->glLineWidth2");
            f->glDrawArrays(GL_LINES, 0, 2 * 2);
            GLError("GridLayer f->glDrawArrays");

            program->disableAttributeArray("a_position");
        }

        void GridLayer::UpdateScene(std::shared_ptr<Model> const &m,
                                    std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            program->bind();

            this->visible = m->interactive->GridVisible;

            if (m->view->IsChanged() || m->documentAccess->IsChanged())
            {
                this->gridSpacing = m->documentAccess->GetDocument()->GetSceneConf()->Settings.GetGridSpacing();
                this->viewMatrix = m->view->viewMatrix;
                this->worldMatrix = m->view->worldMatrix;

                UpdateLines();

                f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);
                f->glBufferData(GL_ARRAY_BUFFER, this->positions->size() * sizeof(float), this->positions->data(),
                                GL_DYNAMIC_DRAW);

                UpdateOriginLines();

                f->glBindBuffer(GL_ARRAY_BUFFER, this->originPositionsBuffer);
                f->glBufferData(GL_ARRAY_BUFFER, this->originPositions->size() * sizeof(float), this->originPositions->data(),
                                GL_DYNAMIC_DRAW);

                program->setUniformValue("u_view", m->view->viewMatrix);
            }

            if (m->IsChanged())
            {
                program->bind();
                program->setUniformValue("u_color", m->colorScheme->EditorLineColor());
            }
        }

        MinMaxValue GridLayer::GetMinMax()
        {
            MinMaxValue result;
            result.Active = false;
            return result;
        }

        float GridLayer::CalcIdealSpacing()
        {
            QVector3D center = this->worldMatrix * QVector3D(0, 0, 0);
            QVector3D gridSpacingPoint = this->worldMatrix * QVector3D(this->gridSpacing, 0, 0);
            float distance = (center - gridSpacingPoint).length();
            float x = 1;
            while (0.05 > distance * x)
            {
                x *= 2;
            }
            return this->gridSpacing * x;
        }

        void GridLayer::UpdateLines()
        {
            float grid_spacing = CalcIdealSpacing();

            QVector2D tl = (this->viewMatrix.inverted() * QVector3D(-1, 1, 0)).toVector2D();
            QVector2D br = (this->viewMatrix.inverted() * QVector3D(1, -1, 0)).toVector2D();

            tl[0] = floorf(tl[0] / grid_spacing);
            tl[1] = floorf(tl[1] / grid_spacing);
            br[0] = ceilf(br[0] / grid_spacing);
            br[1] = ceilf(br[1] / grid_spacing);

            unsigned int verticalLines = (unsigned int) fabsf(br[0] - tl[0]);
            unsigned int horizontalLines = (unsigned int) fabsf(br[1] - tl[1]);

            unsigned int lines = verticalLines + horizontalLines;

            tl[0] = tl[0] * grid_spacing;
            tl[1] = tl[1] * grid_spacing;
            br[0] = br[0] * grid_spacing;
            br[1] = br[1] * grid_spacing;

            std::unique_ptr<std::vector<float> > positions = std::unique_ptr<std::vector<float> >(
                    new std::vector<float>(lines * 4));
            for (int i = 0; i < verticalLines; i++)
            {
                (*positions)[i * 4 + 0] = tl[0] + i * grid_spacing;
                (*positions)[i * 4 + 1] = tl[1];
                (*positions)[i * 4 + 2] = tl[0] + i * grid_spacing;
                (*positions)[i * 4 + 3] = br[1];
            }

            for (int i = 0; i < horizontalLines; i++)
            {
                int offset = verticalLines * 4;
                (*positions)[offset + i * 4 + 0] = tl[0];
                (*positions)[offset + i * 4 + 1] = tl[1] + i * grid_spacing;
                (*positions)[offset + i * 4 + 2] = br[0];
                (*positions)[offset + i * 4 + 3] = tl[1] + i * grid_spacing;
            }

            this->positions = std::move(positions);
            this->lines = lines;
        }

        void GridLayer::UpdateOriginLines()
        {
            QVector2D tl = (this->viewMatrix.inverted() * QVector3D(-1, 1, 0)).toVector2D();
            QVector2D br = (this->viewMatrix.inverted() * QVector3D(1, -1, 0)).toVector2D();
            std::unique_ptr<std::vector<float> > positions = std::unique_ptr<std::vector<float> >(new std::vector<float>());

            positions->push_back(tl[0]); positions->push_back(0);
            positions->push_back(br[0]); positions->push_back(0);

            positions->push_back(0); positions->push_back(tl[1]);
            positions->push_back(0); positions->push_back(br[1]);

            this->originPositions = std::move(positions);
        }


    }
}
