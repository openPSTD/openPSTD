//
// Created by michiel on 30-8-2015.
//

#include "InteractiveLayer.h"
#include "PstdAlgorithm.h"

InteractiveLayer::InteractiveLayer(): addDomainVisible(false), newDomainBuffer(false)
{

}

void InteractiveLayer::InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    QColor color(1.0f, 1.0f, 1.0f, 1.0f);

    f->glGenBuffers(1, &this->newDomainBuffer);
    f->glGenBuffers(1, &this->selectionBuffer);

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
    if(this->addDomainVisible)
    {
        program->bind();

        program->setUniformValue("u_color", newDomainColor);

        program->enableAttributeArray("a_position");
        f->glBindBuffer(GL_ARRAY_BUFFER, this->newDomainBuffer);
        f->glVertexAttribPointer((GLuint)program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);

        f->glLineWidth(5.0f);
        f->glDrawArrays(GL_LINES, 0, 4*2);
        program->disableAttributeArray("a_position");
    }
    if(this->selectionVisible)
    {
        program->bind();

        program->setUniformValue("u_color", selectionColor);

        program->enableAttributeArray("a_position");
        f->glBindBuffer(GL_ARRAY_BUFFER, this->selectionBuffer);
        f->glVertexAttribPointer((GLuint)program->attributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);

        f->glLineWidth(1.0f);
        f->glDrawArrays(GL_LINES, 0, 4*2);
        program->disableAttributeArray("a_position");
    }
}

void InteractiveLayer::UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f)
{
    if(m->interactive->IsChanged() || m->d->IsChanged())
    {
        this->addDomainVisible = m->interactive->CreateDomain.visible;

        if(this->addDomainVisible)
        {
            std::unique_ptr<std::vector<float>> positions(new std::vector<float>());

            AddSquareBuffer(positions, m->interactive->CreateDomain.start, m->interactive->CreateDomain.currentEnd - m->interactive->CreateDomain.start);

            f->glBindBuffer(GL_ARRAY_BUFFER, this->newDomainBuffer);
            f->glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(float), positions->data(), GL_DYNAMIC_DRAW);
        }

        this->selectionVisible = (m->interactive->Selection.Type != SELECTION_NONE);

        if(this->selectionVisible)
        {
            std::shared_ptr<rapidjson::Document> conf = m->d->GetSceneConf();
            int i = m->interactive->Selection.SelectedIndex;

            std::unique_ptr<std::vector<float>> positions(new std::vector<float>());

            if(m->interactive->Selection.Type == SELECTION_DOMAIN)
            {
                rapidjson::Value& domains = (*conf)["domains"];

                QVector2D tl(domains[i]["topleft"][0].GetDouble(), domains[i]["topleft"][1].GetDouble());
                QVector2D size(domains[i]["size"][0].GetDouble(), domains[i]["size"][1].GetDouble());

                AddSquareBuffer(positions, tl, size);
            }
            else if(m->interactive->Selection.Type == SELECTION_RECEIVER)
            {
                rapidjson::Value& receivers = (*conf)["receivers"];

                QVector2D receiver = QVector2D(receivers[i][0].GetDouble(), receivers[i][1].GetDouble());
                QVector2D SizeLines(0.4, 0.4);

                AddSquareBuffer(positions, receiver-SizeLines/2, SizeLines);
            }
            else if(m->interactive->Selection.Type == SELECTION_SPEAKER)
            {
                rapidjson::Value& speakers = (*conf)["speakers"];

                QVector2D speaker = QVector2D(speakers[i][0].GetDouble(), speakers[i][1].GetDouble());
                QVector2D SizeLines(0.4, 0.4);

                AddSquareBuffer(positions, speaker-SizeLines/2, SizeLines);
            }

            f->glBindBuffer(GL_ARRAY_BUFFER, this->selectionBuffer);
            f->glBufferData(GL_ARRAY_BUFFER, positions->size() * sizeof(float), positions->data(), GL_DYNAMIC_DRAW);
        }
    }

    if(m->settings->IsChanged())
    {
        this->newDomainColor = m->settings->visual.colorScheme->EditorAddDomainColor();
        this->selectionColor = m->settings->visual.colorScheme->EditorSelectionColor();

    }

    if(m->view->IsChanged())
    {
        program->bind();
        program->setUniformValue("u_view", m->view->viewMatrix);
    }
}

MinMaxValue InteractiveLayer::GetMinMax()
{
    MinMaxValue result;
    result.Active = false;
    return result;
}