#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QTimer>
#include <QColorDialog>
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
    Settings* settings;
    ~Window();
    
    QSpinBox* sbGridSize;
    QSpinBox* sbZoom;
protected:
    void paintEvent(QPaintEvent* event);
private:
    QWidget* spacer;
    QLabel* lGridSize;
    QLabel* lZoom;
    QActionGroup* qagMainToolbar;
public slots:
    inline void slot_gridsize(int gridsize) { view->renderer->setGridSize(gridsize); }
    inline void slot_zoom(int zoom) { view->model->zoom = zoom; }
    inline void slot_select() { view->renderer->setState(SELECT); }
    inline void slot_move() { view->renderer->setState(MOVE); }
    inline void slot_adddomain() { view->renderer->setState(ADDDOMAIN); }
    inline void slot_addsource() { view->renderer->setState(ADDSOURCE); }
    inline void slot_addreceiver() { view->renderer->setState(ADDRECEIVER); }
    inline void slot_fpscounter() {
        view->model->showFPS = ui->actionFPS_counter->isChecked();
        ui->actionFPS_color->setEnabled(view->model->showFPS);
    }
    void slot_gridcolor();
    void slot_bgcolor();
    void slot_zoomcolor();
    void slot_fpscolor();
};

#endif