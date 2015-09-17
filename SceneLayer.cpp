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
//
//
// Authors:
//
//
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 26-8-2015.
//

#include "SceneLayer.h"

SceneLayer::SceneLayer() : positions(new std::vector<float>()), values(new std::vector<float>()), lines(0),
                           positionsBuffer(), valuesBuffer()
{

}

void SceneLayer::InitializeGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
{
    f->glGenTextures(1, &this->textureID);

    f->glGenBuffers(1, &this->positionsBuffer);
    f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);

    f->glGenBuffers(1, &this->valuesBuffer);
    f->glBindBuffer(GL_ARRAY_BUFFER, this->valuesBuffer);

    std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(new std::string("GPU/Scene2D.vert"));
    std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(new std::string("GPU/Scene2D.frag"));

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
    f->glVertexAttribPointer((GLuint)program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);

    program->enableAttributeArray("a_value");
    f->glBindBuffer(GL_ARRAY_BUFFER, this->valuesBuffer);
    f->glVertexAttribPointer((GLuint)program->attributeLocation("a_value"), 1, GL_FLOAT, GL_FALSE, 0, 0);

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_1D, this->textureID);
    f->glUniform1i((GLuint)program->attributeLocation("colormap"), GL_TEXTURE0);

    f->glLineWidth(5.0f);
    f->glDrawArrays(GL_LINES, 0, lines*2);
    program->disableAttributeArray("a_position");
    program->disableAttributeArray("a_value");
}

void SceneLayer::UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    CreateColormap(m, f);

    program->bind();
    if(m->view->IsChanged())
    {
        program->setUniformValue("u_view", m->view->viewMatrix);
    }

    if(m->settings->IsChanged())
    {
        this->lineWidth = m->settings->visual.EdgeSize;
    }

    if(m->d->IsChanged())
    {
        std::shared_ptr<rapidjson::Document> conf = m->d->GetSceneConf();

        rapidjson::Value &domains = (*conf)["domains"];

        this->positions->clear();
        this->values->clear();
        this->lines = 0;

        for (rapidjson::SizeType i = 0; i < domains.Size(); i++)
        {
            QVector2D tl(domains[i]["topleft"][0].GetDouble(), domains[i]["topleft"][1].GetDouble());
            QVector2D size(domains[i]["size"][0].GetDouble(), domains[i]["size"][1].GetDouble());
            QVector2D br = tl + size;
            float left = tl[0];
            float top = tl[1];

            float right = br[0];
            float bottom = br[1];

            float aTop = domains[i]["edges"]["t"]["a"].GetDouble();
            float aBottom = domains[i]["edges"]["b"]["a"].GetDouble();
            float aLeft = domains[i]["edges"]["l"]["a"].GetDouble();
            float aRight = domains[i]["edges"]["r"]["a"].GetDouble();

            this->positions->push_back(left);
            this->positions->push_back(top);
            this->values->push_back(aTop);

            this->positions->push_back(right);
            this->positions->push_back(top);
            this->values->push_back(aTop);


            this->positions->push_back(left);
            this->positions->push_back(top);
            this->values->push_back(aLeft);

            this->positions->push_back(left);
            this->positions->push_back(bottom);
            this->values->push_back(aLeft);


            this->positions->push_back(left);
            this->positions->push_back(bottom);
            this->values->push_back(aBottom);

            this->positions->push_back(right);
            this->positions->push_back(bottom);
            this->values->push_back(aBottom);


            this->positions->push_back(right);
            this->positions->push_back(top);
            this->values->push_back(aRight);

            this->positions->push_back(right);
            this->positions->push_back(bottom);
            this->values->push_back(aRight);

            this->lines += 4;
        }

        f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);
        f->glBufferData(GL_ARRAY_BUFFER, this->positions->size() * sizeof(float), this->positions->data(),
                        GL_DYNAMIC_DRAW);

        f->glBindBuffer(GL_ARRAY_BUFFER, this->valuesBuffer);
        f->glBufferData(GL_ARRAY_BUFFER, this->values->size() * sizeof(float), this->values->data(), GL_DYNAMIC_DRAW);
    }
}

MinMaxValue SceneLayer::GetMinMax()
{
    return MinMaxValue();
}

void SceneLayer::CreateColormap(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
{
    if(m->settings->IsChanged())
    {
        std::unique_ptr<std::vector<float>> colormap = m->settings->visual.colorScheme->
                EditorLineAbsoptionColorGradient()->CreateRawRGBAColorMap(0, 1, 512);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        f->glBindTexture(GL_TEXTURE_1D, this->textureID);

        // Give the image to OpenGL
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 512, 0, GL_RGBA, GL_FLOAT, colormap->data());

        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        f->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}