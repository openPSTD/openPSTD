//
// Created by michiel on 26-8-2015.
//

#include "GridLayer.h"

GridLayer::GridLayer(): positionsBuffer(QOpenGLBuffer::VertexBuffer)
{

}

void GridLayer::InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    QColor color(255, 255, 255, 255);

    gridSpacing = 0.2f;

    positionsBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    positionsBuffer.create();

    std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(new std::string("GPU/Grid.vert.glsl"));
    std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(new std::string("GPU/Grid.frag.glsl"));

    program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
    program->link();

    program->bind();

    program->setUniformValue("u_color", color);
}

void GridLayer::PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    program->bind();
    program->enableAttributeArray("a_position");
    f->glLineWidth(1.0f);
    f->glDrawArrays(GL_LINES, 0, lines*2);
    program->disableAttributeArray("a_position");
}

void GridLayer::UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    this->gridSpacing = (*m->d->GetSceneConf())["grid_spacing"].GetDouble();
    if(m->view->IsChanged())
    {
        UpdateLines();

        positionsBuffer.bind();
        f->glVertexAttribPointer((GLuint)program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
        positionsBuffer.allocate(this->positions->data(), this->positions->size()*sizeof(float));

        program->bind();
        program->setUniformValue("u_view", this->viewMatrix);
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
    QVector3D center = this->viewMatrix * QVector3D(0, 0, 0);
    QVector3D gridSpacingPoint = this->viewMatrix * QVector3D(this->gridSpacing, 0, 0);
    float distance = (center - gridSpacingPoint).length();
    float x = 1;
    while(0.04 > distance * x)
    {
        x *= 2;
    }
    return this->gridSpacing * x;
}

void GridLayer::UpdateLines()
{
    float grid_spacing = CalcIdealSpacing();

    QVector2D tl = (this->viewMatrix.inverted() * QVector3D(-1, -1, 0)).toVector2D();
    QVector2D br = (this->viewMatrix.inverted() * QVector3D(1, 1, 0)).toVector2D();

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
    }

    for(int i = 0; i < horizontalLines; i++)
    {
        int offset = verticalLines*4;
        (*positions)[offset+i*4+0] = tl[0];
        (*positions)[offset+i*4+1] = tl[1]+i*grid_spacing;
        (*positions)[offset+i*4+2] = br[0];
        (*positions)[offset+i*4+3] = tl[1]+i*grid_spacing;
    }

    this->positions = std::move(positions);
    this->lines = lines;
}