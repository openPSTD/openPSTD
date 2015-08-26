//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_VIEWER2D_H
#define OPENPSTD_VIEWER2D_H

#include <QOpenGLWidget>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <algorithm>
#include "Model.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "operations/BaseOperation.h"
#include <QOpenGLBuffer>

class Layer;

class MinMaxValue
{
public:
    MinMaxValue();
    MinMaxValue(QVector2D min, QVector2D max);

    QVector2D min;
    QVector2D max;
    bool Active;

    static MinMaxValue Combine(MinMaxValue first, MinMaxValue second);

    static MinMaxValue CombineList(std::vector<MinMaxValue> list);
};

void DeleteNothing(void * ptr);
void DeleteTexture(void * ptr);

class Viewer2D: public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit Viewer2D(QWidget *parent = 0);
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;
    void UpdateViewMatrix(QMatrix4x4 matrix);
    QMatrix4x4 GetViewMatrix(){return _view_matrix;}
    void UpdateFromModel(std::shared_ptr<Model> const &model);
    void SetOperationRunner(std::shared_ptr<OperationRunner> operationRunner);


protected:
    virtual void resizeGL(int w, int h) override;
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void mousePressEvent(QMouseEvent *mouseEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent *mouseEvent) override;
    virtual void mouseMoveEvent(QMouseEvent *mouseEvent) override;
    virtual void wheelEvent(QWheelEvent *qWheelEvent) override;

private:
    std::vector<std::shared_ptr<Layer>> layers;
    QMatrix4x4 _view_matrix;
    std::shared_ptr<OperationRunner> operationRunner;
};

class Layer
{
protected:
    QMatrix4x4 viewMatrix;
    bool visible;

public:
    virtual bool GetVisible() { return visible; };
    virtual void SetVisible(bool value) { visible = value; };

    virtual void InitializeGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f) = 0;
    virtual void PaintGLVisibilityCheck(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f){ if(visible){ this->PaintGL(context, f); } };
    virtual void PaintGL(QObject* context, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f) = 0;
    virtual void UpdateScene(std::shared_ptr<Model> const &m, std::unique_ptr<QOpenGLFunctions, void(*)(void*)> const &f) = 0;
    virtual MinMaxValue GetMinMax() = 0;
    virtual void UpdateViewMatrix(QMatrix4x4 viewMatrix){ this->viewMatrix = viewMatrix; };
};

class GridLayer: public Layer
{
private:
    std::unique_ptr<QOpenGLShaderProgram> program;
    void UpdateLines();
    std::unique_ptr<std::vector<float>> positions;
    QOpenGLBuffer positionsBuffer;
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

#endif //OPENPSTD_VIEWER2D_H
