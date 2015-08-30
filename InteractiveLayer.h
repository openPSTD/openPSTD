//
// Created by michiel on 30-8-2015.
//

#ifndef OPENPSTD_INTERACTIVELAYER_H
#define OPENPSTD_INTERACTIVELAYER_H

#include "Viewer2D.h"

class InteractiveLayer : public Layer
{
private:
    std::unique_ptr<QOpenGLShaderProgram> program;
    bool AddDomainVisible;
    unsigned int positionsBuffer;
    int lines;
public:
    InteractiveLayer();

    virtual void InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f);

    virtual void PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f);

    virtual void UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f);

    virtual MinMaxValue GetMinMax();
};


#endif //OPENPSTD_INTERACTIVELAYER_H
