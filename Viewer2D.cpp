//
// Created by michiel on 18-7-2015.
//

#include "Viewer2D.h"
#include <qopenglcontext.h>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <algorithm>
#include <math.h>


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

    static const GLfloat g_vertex_buffer_data[] = {};

    QColor color(255, 255, 0, 255);

    program->enableAttributeArray("a_position");
    program->setUniformValue("u_color", color);
}

void GridLayer::PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    UpdateLines();

    program->bind();
    program->setUniformValue("u_view", this->viewMatrix);
    program->setAttributeArray("a_position", this->positions->data(), 2);
    f->glDrawArrays(GL_LINES, 0, lines*2);
}

void GridLayer::UpdateScene(Model m)
{

}

MinMaxValue GridLayer::GetMinMax()
{
    MinMaxValue result;
    result.Active = false;
    return result;
}

void Viewer2D::UpdateViewMatrix(QMatrix4x4 matrix)
{
    this->_view_matrix = matrix;

    std::for_each(this->layers.begin(), this->layers.end(), [matrix](std::shared_ptr<Layer> l){l->UpdateViewMatrix(matrix);});
}

void GridLayer::UpdateLines()
{
    std::cout << "------------------------------------------------------------" << std::endl;
    float grid_spacing = 0.2f;

    QVector2D tl = (QVector3D(-1, -1, 0)*this->viewMatrix.inverted()).toVector2D();
    QVector2D br = (QVector3D(1, 1, 0)*this->viewMatrix.inverted()).toVector2D();

    tl[0] = floorf(tl[0]/grid_spacing);
    tl[1] = floorf(tl[1]/grid_spacing);
    br[0] = ceilf(br[0]/grid_spacing);
    br[1] = ceilf(br[1]/grid_spacing);

    unsigned int verticalLines = (unsigned int)fabsf(br[0]-tl[0]);
    unsigned int horizontalLines = (unsigned int)fabsf(br[1]-tl[1]);

    unsigned int lines = verticalLines+horizontalLines;

    tl[0] = tl[0]*grid_spacing;
    tl[1] = tl[1]*grid_spacing;
    br[0] = br[0]*grid_spacing;
    br[1] = br[1]*grid_spacing;

    std::unique_ptr<std::vector<float> > positions = std::unique_ptr<std::vector<float> >(new std::vector<float>(lines*4));
    for(int i = 0; i < verticalLines; i++)
    {
        (*positions)[i*4+0] = tl[0]+i*grid_spacing;
        (*positions)[i*4+1] = tl[1];
        (*positions)[i*4+2] = tl[0]+i*grid_spacing;
        (*positions)[i*4+3] = br[1];
        std::cout << "[(" << (*positions)[i*4+0] << "," << (*positions)[i*4+1] << "),(" << (*positions)[i*4+2] << "," << (*positions)[i*4+3] << ")]" << std::endl;
    }

    for(int i = 0; i < horizontalLines; i++)
    {
        int offset = verticalLines*4;
        (*positions)[offset+i*4+0] = tl[0];
        (*positions)[offset+i*4+1] = tl[1]+i*grid_spacing;
        (*positions)[offset+i*4+2] = br[0];
        (*positions)[offset+i*4+3] = tl[1]+i*grid_spacing;
        std::cout << "[(" << (*positions)[offset+i*4+0] << "," << (*positions)[offset+i*4+1] << "),(" << (*positions)[offset+i*4+2] << "," << (*positions)[offset+i*4+3] << ")]" << std::endl;
    }

    this->positions = std::move(positions);
    this->lines = lines;
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
    if(!first.Active && !second.Active)
    {
        result.Active = false;
    }
    else if(!first.Active)
    {
        for (int i = 0; i < 2; i++)
        {
            result.min[i] = second.min[i];
            result.max[i] = second.max[i];
        }
    }
    else if(!second.Active)
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
    for(MinMaxValue v: list)
    {
        result = Combine(result, v);
    }
    return result;
}
