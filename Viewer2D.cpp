//
// Created by michiel on 18-7-2015.
//

#include "Viewer2D.h"

Viewer2D::Viewer2D(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{

}

void Viewer2D::paintGL()
{
    glLoadIdentity();
    glBegin(GL_TRIANGLES);
    qglColor(Qt::red);
    glVertex3f(-1,-1,0);
    qglColor(Qt::blue);
    glVertex3f(-1,1,0);
    qglColor(Qt::yellow);
    glVertex3f(1,1,0);
    glEnd();
}

void Viewer2D::initializeGL()
{
    glClearColor(0, 0, 0, 1);

    glDisable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}

void Viewer2D::resizeGL(int w, int h)
{
    glViewport(0, 0, h, w);
}

QSize Viewer2D::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize Viewer2D::sizeHint() const
{
    return QSize(400, 400);
}
