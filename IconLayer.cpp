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
    f->glGenBuffers(1, &this->ColorBuffer);

    std::unique_ptr<std::string> vertexFile = std::unique_ptr<std::string>(new std::string("GPU/Symbol.vert.glsl"));
    std::unique_ptr<std::string> fragmentFile = std::unique_ptr<std::string>(new std::string("GPU/Symbol.frag.glsl"));

    program = std::unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram(nullptr));
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(*vertexFile));
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(*fragmentFile));
    program->link();

    program->bind();
}

void IconLayer::PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    program->bind();

    program->enableAttributeArray("a_position");
    f->glBindBuffer(GL_ARRAY_BUFFER, this->LineBuffers);
    f->glVertexAttribPointer((GLuint)program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);

    program->enableAttributeArray("a_color");
    f->glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
    f->glVertexAttribPointer((GLuint)program->attributeLocation("a_color"), 4, GL_FLOAT, GL_FALSE, 0, 0);

    f->glLineWidth(1.0f);
    f->glDrawArrays(GL_LINES, 0, lines*2);
    program->disableAttributeArray("a_position");
    program->disableAttributeArray("a_color");
}

void IconLayer::UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    if(m->d->IsChanged())
    {
        QVector2D SizeLines(0.4, 0.4);
        QVector2D SizeSquare(0.2, 0.2);
        std::unique_ptr<std::vector<QVector2D>> positions(new std::vector<QVector2D>());
        std::unique_ptr<std::vector<QVector4D>> colors(new std::vector<QVector4D>());

        std::vector<QVector2D> speakers = GetSpeakers(m);
        for(int i = 0; i < speakers.size(); i++)
        {
            positions->push_back(QVector2D(speakers[i].x()-SizeLines.x()/2, speakers[i].y()));
            colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorSpeakerColor()));
            positions->push_back(QVector2D(speakers[i].x()+SizeLines.x()/2, speakers[i].y()));
            colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorSpeakerColor()));

            positions->push_back(QVector2D(speakers[i].x(), speakers[i].y()-SizeLines.y()/2));
            colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorSpeakerColor()));
            positions->push_back(QVector2D(speakers[i].x(), speakers[i].y()+SizeLines.y()/2));
            colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorSpeakerColor()));

            AddSquareBuffer(positions, speakers[i]-SizeSquare/2, SizeSquare);
            for(int j = 0; j < 8; j++) colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorSpeakerColor()));
        }

        std::vector<QVector2D> receivers = GetReceivers(m);
        for(int i = 0; i < receivers.size(); i++)
        {
            positions->push_back(QVector2D(receivers[i].x()-SizeLines.x()/2, receivers[i].y()));
            colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorReceiverColor()));
            positions->push_back(QVector2D(receivers[i].x()+SizeLines.x()/2, receivers[i].y()));
            colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorReceiverColor()));

            positions->push_back(QVector2D(receivers[i].x(), receivers[i].y()-SizeLines.y()/2));
            colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorReceiverColor()));
            positions->push_back(QVector2D(receivers[i].x(), receivers[i].y()+SizeLines.y()/2));
            colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorReceiverColor()));

            AddSquareBuffer(positions, receivers[i]-SizeSquare/2, SizeSquare);
            for(int j = 0; j < 8; j++) colors->push_back(ToVectorRGBA(m->settings->visual.colorScheme->EditorReceiverColor()));
        }


        this->lines = speakers.size()*6 + receivers.size()*6;

        f->glBindBuffer(GL_ARRAY_BUFFER, this->LineBuffers);
        f->glBufferData(GL_ARRAY_BUFFER, positions->size()*sizeof(QVector2D), positions->data(), GL_DYNAMIC_DRAW);
        f->glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
        f->glBufferData(GL_ARRAY_BUFFER, colors->size()*sizeof(QVector4D), colors->data(), GL_DYNAMIC_DRAW);
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

std::vector<QVector2D> IconLayer::GetReceivers(std::shared_ptr<Model> const &m)
{
    std::vector<QVector2D> result;
    std::shared_ptr<rapidjson::Document> conf = m->d->GetSceneConf();

    rapidjson::Value &receivers = (*conf)["receivers"];
    for (rapidjson::SizeType i = 0; i < receivers.Size(); i++)
    {
        result.push_back(QVector2D(receivers[i][0].GetDouble(), receivers[i][1].GetDouble()));
    }
    return result;
}