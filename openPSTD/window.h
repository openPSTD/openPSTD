#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QTimer>
#include "renderer.h"
#include "graphicsview.h"
#include "ui_window.h"

namespace Ui {
class Window;
}

class Window : public QMainWindow {
    Q_OBJECT
public:
    explicit Window(QWidget* parent = 0);
    Ui::Window* ui;
    GraphicsView* view;
    ~Window();
protected:
    void paintEvent(QPaintEvent* event);
private:
    QSpinBox* sbGridSize;
public slots:
    inline void slot_gridsize(int gridsize) { view->renderer->setGridSize(gridsize); }
    inline void slot_select() { view->renderer->setState(SELECT); }
    inline void slot_move() { view->renderer->setState(MOVE); }
    inline void slot_adddomain() { view->renderer->setState(ADDDOMAIN); }
    inline void slot_addsource() { view->renderer->setState(ADDSOURCE); }
    inline void slot_addreceiver() { view->renderer->setState(ADDRECEIVER); }
};

#endif