//
// Created by michiel on 26-8-2015.
//

#include "SceneLayer.h"

SceneLayer::SceneLayer() : positions(new std::vector<float>()), values(new std::vector<float>()), lines(0),
                           positionsBuffer(QOpenGLBuffer::VertexBuffer), valuesBuffer(QOpenGLBuffer::VertexBuffer)
{

}

void SceneLayer::InitializeGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
{
    positionsBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    positionsBuffer.create();

    valuesBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    valuesBuffer.create();

    std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(new std::string("GPU/Scene2D.vert"));
    std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(new std::string("GPU/Scene2D.frag"));

    program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
    program->link();

    program->bind();

    CreateColormap();


    program->setUniformValue("colormap", this->textureID);
    program->setUniformValue("vmin", 0.0f);
    program->setUniformValue("vmax", 1.0f);


}

void SceneLayer::PaintGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f)
{
    program->bind();
    program->enableAttributeArray("a_position");
    program->enableAttributeArray("a_value");
    f->glLineWidth(5.0f);
    f->glDrawArrays(GL_LINES, 0, lines*2);
    program->disableAttributeArray("a_position");
    program->disableAttributeArray("a_value");
}

void SceneLayer::UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    if(m->view->IsChanged())
    {
        program->bind();
        program->setUniformValue("u_view", this->viewMatrix);
    }

    std::shared_ptr<rapidjson::Document> conf = m->d->GetSceneConf();

    rapidjson::Value& domains = (*conf)["domains"];

    this->positions->clear();
    this->values->clear();
    this->lines = 0;

    for(rapidjson::SizeType i = 0; i < domains.Size(); i++)
    {
        QVector2D tl(domains[i]["topleft"][0].GetDouble(), domains[i]["topleft"][1].GetDouble());
        QVector2D size(domains[i]["size"][0].GetDouble(), domains[i]["size"][1].GetDouble());
        QVector2D br = tl+size;
        float left = tl[0];
        float top = tl[1];

        float right = br[0];
        float bottom = br[1];

        float aTop = domains[i]["edges"]["t"]["a"].GetDouble();
        float aBottom = domains[i]["edges"]["b"]["a"].GetDouble();
        float aLeft = domains[i]["edges"]["l"]["a"].GetDouble();
        float aRight = domains[i]["edges"]["r"]["a"].GetDouble();

        this->positions->push_back(left); this->positions->push_back(top);
        this->values->push_back(aTop);
        this->positions->push_back(right); this->positions->push_back(top);
        this->values->push_back(aTop);

        this->positions->push_back(left); this->positions->push_back(top);
        this->values->push_back(aLeft);
        this->positions->push_back(left); this->positions->push_back(bottom);
        this->values->push_back(aLeft);

        this->positions->push_back(left); this->positions->push_back(bottom);
        this->values->push_back(aBottom);
        this->positions->push_back(right); this->positions->push_back(bottom);
        this->values->push_back(aBottom);

        this->positions->push_back(right); this->positions->push_back(top);
        this->values->push_back(aRight);
        this->positions->push_back(right); this->positions->push_back(bottom);
        this->values->push_back(aRight);

        this->lines += 4;
    }

    positionsBuffer.bind();
    f->glVertexAttribPointer((GLuint)program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
    positionsBuffer.allocate(this->positions->data(), this->positions->size()*sizeof(float));

    valuesBuffer.bind();
    f->glVertexAttribPointer((GLuint)program->attributeLocation("a_value"), 1, GL_FLOAT, GL_FALSE, 0, 0);
    valuesBuffer.allocate(this->values->data(), this->values->size()*sizeof(float));
}

MinMaxValue SceneLayer::GetMinMax()
{
    return MinMaxValue();
}

void SceneLayer::CreateColormap()
{
    std::unique_ptr<std::vector<float>> colormap(new std::vector<float>(2*512*4));
    for(int i = 0; i < 512; i++)
    {
        (*colormap)[i*4+0] = i/512.0f;
        (*colormap)[i*4+1] = 0;
        (*colormap)[i*4+2] = 1-i/512.0f;
        (*colormap)[i*4+3] = 1;

        (*colormap)[512*4+i*4+0] = i/512.0f;
        (*colormap)[512*4+i*4+1] = 0;
        (*colormap)[512*4+i*4+2] = 1-i/512.0f;
        (*colormap)[512*4+i*4+3] = 1;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512, 2, 0, GL_RGBA, GL_FLOAT, colormap->data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    this->textureID = textureID;
}