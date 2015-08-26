//
// Created by michiel on 26-8-2015.
//

#ifndef OPENPSTD_SCENELAYER_H
#define OPENPSTD_SCENELAYER_H

#include "Viewer2D.h"

class SceneLayer: public Layer
{
private:
    std::unique_ptr<QOpenGLShaderProgram> program;
    QOpenGLBuffer positionsBuffer;
    QOpenGLBuffer valuesBuffer;
    std::unique_ptr<std::vector<float>> positions;
    std::unique_ptr<std::vector<float>> values;
    GLuint textureID;
    int lines;

    void CreateColormap();

public:
    SceneLayer();

    virtual void InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f);

    virtual void PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f);

    virtual void UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f);

    virtual MinMaxValue GetMinMax();
};


#endif //OPENPSTD_SCENELAYER_H
