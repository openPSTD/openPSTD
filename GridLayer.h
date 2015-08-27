//
// Created by michiel on 26-8-2015.
//

#ifndef OPENPSTD_GRIDLAYER_H
#define OPENPSTD_GRIDLAYER_H

#include "Viewer2D.h"

class GridLayer: public Layer
{
private:
    std::unique_ptr<QOpenGLShaderProgram> program;
    void UpdateLines();
    std::unique_ptr<std::vector<float>> positions;
    unsigned int positionsBuffer;
    int lines;
    float gridSpacing;

public:
    GridLayer();

    virtual void InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f);

    virtual void PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f);

    virtual void UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f);

    virtual MinMaxValue GetMinMax();

    float CalcIdealSpacing();
};


#endif //OPENPSTD_GRIDLAYER_H
