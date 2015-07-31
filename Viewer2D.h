//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_VIEWER2D_H
#define OPENPSTD_VIEWER2D_H

#include <QGLWidget>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include <algorithm>
#include "Model.h"

class Viewer2D: public QGLWidget
{
    Q_OBJECT

public:
    explicit Viewer2D(QWidget *parent = 0);

public:
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

protected:
    virtual void resizeGL(int w, int h) override;
    virtual void initializeGL() override;
    virtual void paintGL() override;
};

template<typename T>
class MinMaxValue
{
public:
    MinMaxValue() {}
    MinMaxValue(boost::numeric::ublas::vector<T> min, boost::numeric::ublas::vector<T> max)
    {
        if(min.size() != max.size())
        {
            //todo throw an exception
        }
        this->min = min;
        this->max = max;
    }

    boost::numeric::ublas::vector<T> min;
    boost::numeric::ublas::vector<T> max;

    unsigned int GetDimensions()
    {
        return min.size();
    }


    static MinMaxValue<T> Combine(MinMaxValue<T> first, MinMaxValue<T> second)
    {
        using namespace boost::numeric::ublas;
        MinMaxValue<T> result;
        result.min = vector<T>(first.GetDimensions());
        result.max = vector<T>(first.GetDimensions());
        for(int i = 0; i < first.GetDimensions(); i++)
        {
            result.min(i) = std::min(first.min(i), second.min(i));
            result.max(i) = std::max(first.max(i), second.max(i));
        }
        return result;
    }

    static MinMaxValue<T> CombineList(std::vector<MinMaxValue<T>> list)
    {
        MinMaxValue<T> result = list.back();
        list.pop_back();
        for(MinMaxValue<T> v: list)
        {
            result = Combine(result, v);
        }
        return result;
    }
};

class Layer
{
protected:
    Viewer2D viewer2D;
    std::shared_ptr<boost::numeric::ublas::matrix<float> > viewMatrix;
    bool visible;

public:
    virtual bool GetVisible() { return visible; };
    virtual void SetVisible(bool value) { visible = value; };

    virtual void InitializeGL() = 0;
    virtual void PaintGLVisibilityCheck(){ if(visible){ this->PaintGL(); } };
    virtual void PaintGL() = 0;
    virtual void UpdateScene(Model m) = 0;
    virtual MinMaxValue GetMinMax() = 0;
    virtual void UpdateViewMatrix(std::shared_ptr<boost::numeric::ublas::matrix<float> > viewMatrix){ this->viewMatrix = viewMatrix; };
};


#endif //OPENPSTD_VIEWER2D_H
