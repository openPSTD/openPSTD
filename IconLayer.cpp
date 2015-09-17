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
// Created by michiel on 10-9-2015.
//

#include "IconLayer.h"
#include "PstdAlgorithm.h"

IconLayer::IconLayer(): lines(0)
{

}

void IconLayer::InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    QColor color(0, 255, 255, 255);

    f->glGenBuffers(1, &this->LineBuffers);

    std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(new std::string("GPU/Grid.vert.glsl"));
    std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(new std::string("GPU/Grid.frag.glsl"));

    program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
    program->link();

    program->bind();

    program->setUniformValue("u_color", color);
}

void IconLayer::PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    program->bind();
    program->enableAttributeArray("a_position");
    f->glBindBuffer(GL_ARRAY_BUFFER, this->LineBuffers);
    f->glVertexAttribPointer((GLuint)program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);

    f->glLineWidth(1.0f);
    f->glDrawArrays(GL_LINES, 0, lines*2);
    program->disableAttributeArray("a_position");
}

void IconLayer::UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    if(m->d->IsChanged())
    {
        QVector2D SizeLines(0.4, 0.4);
        QVector2D SizeSquare(0.2, 0.2);
        std::unique_ptr<std::vector<QVector2D>> positions(new std::vector<QVector2D>());
        std::vector<QVector2D> speakers = GetSpeakers(m);
        for(int i = 0; i < speakers.size(); i++)
        {
            positions->push_back(QVector2D(speakers[i].x()-SizeLines.x()/2, speakers[i].y()));
            positions->push_back(QVector2D(speakers[i].x()+SizeLines.x()/2, speakers[i].y()));

            positions->push_back(QVector2D(speakers[i].x(), speakers[i].y()-SizeLines.y()/2));
            positions->push_back(QVector2D(speakers[i].x(), speakers[i].y()+SizeLines.y()/2));
            AddSquareBuffer(positions, speakers[i]-SizeSquare/2, SizeSquare);
        }
        this->lines = speakers.size()*6;

        f->glBindBuffer(GL_ARRAY_BUFFER, this->LineBuffers);
        f->glBufferData(GL_ARRAY_BUFFER, positions->size()*sizeof(QVector2D), positions->data(), GL_DYNAMIC_DRAW);
    }

    if(m->view->IsChanged())
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
    std::shared_ptr<rapidjson::Document> conf = m->d->GetSceneConf();

    rapidjson::Value &speakers = (*conf)["speakers"];
    for (rapidjson::SizeType i = 0; i < speakers.Size(); i++)
    {
        result.push_back(QVector2D(speakers[i][0].GetDouble(), speakers[i][1].GetDouble()));
    }
    return result;
}