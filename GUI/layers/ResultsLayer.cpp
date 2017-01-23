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
// Date: 16-4-2016
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#include "ResultsLayer.h"
#include <kernel/MockKernel.h>
#include <queue>

namespace OpenPSTD
{
    namespace GUI
    {
        ResultsLayer::ResultsLayer()
        {

        }

        void ResultsLayer::InitializeGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Simulation2D.vert"));
            std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(
                    new std::string(":/GPU/Simulation2D.frag"));

            program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
            program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
            program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
            program->link();

            program->bind();

            program->setUniformValue("vmin", -1.0f);
            program->setUniformValue("vmax", 1.0f);
            program->setUniformValue("v_log_min", -60.0f);
            f->glUniform1i((GLuint) program->attributeLocation("values"), 0);

            std::vector<QVector2D> texCoords;
            texCoords.push_back(QVector2D(0,0));
            texCoords.push_back(QVector2D(0,1));
            texCoords.push_back(QVector2D(1,0));
            texCoords.push_back(QVector2D(1,1));
            f->glGenBuffers(1, &this->texCoordsBuffer);
            f->glBindBuffer(GL_ARRAY_BUFFER, this->texCoordsBuffer);
            f->glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(QVector2D), texCoords.data(),
                            GL_DYNAMIC_DRAW);
        }

        void ResultsLayer::PaintGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            if(this->RenderInfo.size() == 0)
                return;

            program->bind();
            program->enableAttributeArray("a_position");
            program->enableAttributeArray("a_texcoord");

            //tex_coords
            f->glBindBuffer(GL_ARRAY_BUFFER, this->texCoordsBuffer);
            f->glVertexAttribPointer((GLuint) program->attributeLocation("a_texcoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);

            for(auto info : this->RenderInfo)
            {
                //bind values texture of subdomain
                f->glActiveTexture(GL_TEXTURE0);
                f->glBindTexture(GL_TEXTURE_2D, info.texture);

                //positions of sub domain
                f->glBindBuffer(GL_ARRAY_BUFFER, info.positionsBuffer);
                f->glVertexAttribPointer((GLuint) program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);

                f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }

            program->disableAttributeArray("a_texcoord");
            program->disableAttributeArray("a_position");
            program->release();
        }

        void ResultsLayer::UpdateScene(std::shared_ptr<Model> const &m,
                                       std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
        {
            program->bind();
            if (m->view->IsChanged())
            {
                program->setUniformValue("u_view", m->view->viewMatrix);
            }

            if (m->documentAccess->IsChanged() || m->interactive->IsChanged())
            {
                program->bind();
                program->setUniformValue("v_log_min", m->interactive->MinLogScale);

                auto doc = m->documentAccess->GetDocument();
                auto conf = doc->GetResultsSceneConf();
                int frame = m->interactive->visibleFrame;

                Kernel::SimulationMetadata metadata;
                {
                    Kernel::MockKernel k;
                    k.initialize_kernel(conf, std::make_shared<OpenPSTD::Kernel::KernelCallbackLog>());
                    metadata = k.get_metadata();
                }

                //store buffers in queue for re-use. Using queues, because in most cases the former sizes matches the
                //new sizes, this will speed up the reusage.
                std::queue<GLuint> ReUsePosBuffer;
                std::queue<GLuint> ReUseTexture;
                for(int i = 0; i < this->RenderInfo.size(); i++)
                {
                    ReUsePosBuffer.push(this->RenderInfo[i].positionsBuffer);
                    ReUseTexture.push(this->RenderInfo[i].texture);
                }

                this->RenderInfo.clear();
                if(frame >= 0)
                {
                    for (int i = 0; i < doc->GetResultsDomainCount(); i++)
                    {
                        int frameCount = doc->GetResultsFrameCount(i);
                        if (frame < frameCount)
                        {
                            DomainGLInfo info;

                            //create positions buffer
                            QVector2D pos(metadata.DomainPositions[i][0], metadata.DomainPositions[i][1]);
                            QVector2D size(metadata.DomainMetadata[i][0], metadata.DomainMetadata[i][1]);
                            pos *= conf->Settings.GetGridSpacing();
                            size *= conf->Settings.GetGridSpacing();

                            std::vector<QVector2D> worldPos;
                            worldPos.push_back(pos + QVector2D(0, 0) * size);
                            worldPos.push_back(pos + QVector2D(0, 1) * size);
                            worldPos.push_back(pos + QVector2D(1, 0) * size);
                            worldPos.push_back(pos + QVector2D(1, 1) * size);

                            if (ReUsePosBuffer.size() > 0)
                            {
                                info.positionsBuffer = ReUsePosBuffer.front();
                                ReUsePosBuffer.pop();
                            }
                            else
                            {
                                f->glGenBuffers(1, &info.positionsBuffer);
                            }

                            f->glBindBuffer(GL_ARRAY_BUFFER, info.positionsBuffer);
                            f->glBufferData(GL_ARRAY_BUFFER, worldPos.size() * sizeof(QVector2D), worldPos.data(),
                                            GL_DYNAMIC_DRAW);

                            //create the values texture
                            Kernel::PSTD_FRAME_PTR values = doc->GetResultsFrame(frame, i);

                            if (ReUsePosBuffer.size() > 0)
                            {
                                info.texture = ReUseTexture.front();
                                ReUseTexture.pop();
                            }
                            else
                            {
                                f->glGenTextures(1, &info.texture);
                            }

                            f->glActiveTexture(GL_TEXTURE1);
                            f->glBindTexture(GL_TEXTURE_2D, info.texture);
                            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, metadata.DomainMetadata[i][0],
                                         metadata.DomainMetadata[i][1], 0, GL_RED, GL_FLOAT, values->data());

                            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                            f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

                            this->RenderInfo.push_back(info);
                        }
                    }
                }

                //delete the unused buffers and textures(most of the time this is none)
                while(ReUsePosBuffer.size() > 0)
                {
                    GLuint b = ReUsePosBuffer.front();
                    ReUsePosBuffer.pop();
                    f->glDeleteBuffers(1, &b);
                }

                while(ReUseTexture.size() > 0)
                {
                    GLuint t = ReUseTexture.front();
                    ReUseTexture.pop();
                    f->glDeleteTextures(1, &t);
                }
            }
        }

        MinMaxValue ResultsLayer::GetMinMax()
        {
            return GUI::MinMaxValue();
        }
    }
}
