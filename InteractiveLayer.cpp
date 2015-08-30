//
// Created by michiel on 30-8-2015.
//

#include "InteractiveLayer.h"

InteractiveLayer::InteractiveLayer(): AddDomainVisible(false)
{

}

void InteractiveLayer::InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    QColor color(255, 255, 255, 255);

    f->glGenBuffers(1, &this->positionsBuffer);
    f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);

    std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(new std::string("GPU/Interactive.vert.glsl"));
    std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(new std::string("GPU/Interactive.frag.glsl"));

    program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
    program->link();

    program->bind();

    program->setUniformValue("u_color", color);
}

void InteractiveLayer::PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    if(this->AddDomainVisible)
    {
        program->bind();

        program->enableAttributeArray("a_position");
        f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);
        f->glVertexAttribPointer((GLuint)program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);

        f->glLineWidth(5.0f);
        f->glDrawArrays(GL_LINES, 0, lines*2);
        program->disableAttributeArray("a_position");
    }
}

void InteractiveLayer::UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    if(m->interactive->IsChanged())
    {
        std::unique_ptr<std::vector<float>> positions(new std::vector<float>());

        this->AddDomainVisible = m->interactive->CreateDomain.visible;

        float left = m->interactive->CreateDomain.start[0];
        float top = m->interactive->CreateDomain.start[1];

        float right = m->interactive->CreateDomain.currentEnd[0];
        float bottom = m->interactive->CreateDomain.currentEnd[1];

        positions->push_back(left); positions->push_back(top);
        positions->push_back(right); positions->push_back(top);

        positions->push_back(left); positions->push_back(top);
        positions->push_back(left); positions->push_back(bottom);

        positions->push_back(left); positions->push_back(bottom);
        positions->push_back(right); positions->push_back(bottom);

        positions->push_back(right); positions->push_back(top);
        positions->push_back(right); positions->push_back(bottom);

        f->glBindBuffer(GL_ARRAY_BUFFER, this->positionsBuffer);
        f->glBufferData(GL_ARRAY_BUFFER, positions->size()*sizeof(float), positions->data(), GL_DYNAMIC_DRAW);
        this->lines = 4;
    }

    if(m->colorScheme->IsChanged())
    {
        program->bind();
        program->setUniformValue("u_color", m->colorScheme->value->EditorAddDomainColor());
    }

    if(m->view->IsChanged())
    {
        program->bind();
        program->setUniformValue("u_view", this->viewMatrix);
    }
}

MinMaxValue InteractiveLayer::GetMinMax()
{
    MinMaxValue result;
    result.Active = false;
    return result;
}