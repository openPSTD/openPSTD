//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_VIEWER2D_H
#define OPENPSTD_VIEWER2D_H

#include <QGLWidget>

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


#endif //OPENPSTD_VIEWER2D_H
