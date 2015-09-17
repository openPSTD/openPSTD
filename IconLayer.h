//
// Created by michiel on 10-9-2015.
//

#ifndef OPENPSTD_ICONLAYER_H
#define OPENPSTD_ICONLAYER_H

#include "Viewer2D.h"

class IconLayer: public Layer
{
private:
    std::unique_ptr<QOpenGLShaderProgram> program;
    unsigned int LineBuffers;
    unsigned int ColorBuffer;
    unsigned int lines;

    std::vector<QVector2D> GetSpeakers(std::shared_ptr<Model> const &m);
    std::vector<QVector2D> GetReceivers(std::shared_ptr<Model> const &m);

public:
    IconLayer();
    virtual void InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f) override;

    virtual void PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f) override;

    virtual void UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f) override;

    virtual MinMaxValue GetMinMax() override;
};


#endif //OPENPSTD_ICONLAYER_H
