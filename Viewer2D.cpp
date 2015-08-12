//
// Created by michiel on 18-7-2015.
//

#include "Viewer2D.h"
#include <qopenglcontext.h>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>


void DeleteNothing(void * ptr)
{

}

Viewer2D::Viewer2D(QWidget *parent)
    : QOpenGLWidget(parent), layers()
{

}

void Viewer2D::paintGL()
{
    auto del = [](int * p) { std::cout << "Deleting x, value is : " << *p; };
    std::unique_ptr<QOpenGLFunctions, void(*)(void*)> f(QOpenGLContext::currentContext()->functions(), DeleteNothing);

    for(int i = 0; i < this->layers.size(); i++)
    {
        this->layers[i]->PaintGL(this, f);
    }
}

void Viewer2D::initializeGL()
{
    std::unique_ptr<QOpenGLFunctions, void(*)(void*)> f(QOpenGLContext::currentContext()->functions(), DeleteNothing);

    f->glClearColor(0, 0, 0, 1);

    f->glDisable(GL_CULL_FACE);

    std::cout << "create layers" << std::endl;
    this->layers.push_back(std::shared_ptr<Layer>(new GridLayer()));
    for(int i = 0; i < this->layers.size(); i++)
    {
        this->layers[i]->InitializeGL(this, f);
    }
}

void Viewer2D::resizeGL(int w, int h)
{
    std::unique_ptr<QOpenGLFunctions, void(*)(void*)> f(QOpenGLContext::currentContext()->functions(), DeleteNothing);
    f->glViewport(0, 0, h, w);
}

QSize Viewer2D::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize Viewer2D::sizeHint() const
{
    return QSize(400, 400);
}


void GridLayer::InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(new std::string("GPU\\Grid.vert.glsl"));
    std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(new std::string("GPU\\Grid.frag.glsl"));

    program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
    program->link();

    program->bind();

    static const GLfloat g_vertex_buffer_data[] = {
            -1,-1,1,1,
            -1,1,1,-1,
             -0.5f,  0.5f,
             -0.5f,  -0.5f,
             0.5f,  0.5f,
             0.5f,  -0.5f,
    };

    QColor color(255, 255, 0, 255);

    QMatrix3x3 pmvMatrix;
    pmvMatrix.setToIdentity();

    program->enableAttributeArray("a_position");

    program->setAttributeArray("a_position", g_vertex_buffer_data, 2);
    program->setUniformValue("u_color", color);
    program->setUniformValue("u_view", pmvMatrix);
}

void GridLayer::PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    program->bind();
    f->glDrawArrays(GL_LINES, 0, 8);
}

void GridLayer::UpdateScene(Model m)
{

}

MinMaxValue<float> GridLayer::GetMinMax()
{
    return MinMaxValue<float>();
}
