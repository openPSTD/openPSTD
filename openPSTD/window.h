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
#include <QInputDialog>
#include <QMessageBox>
#include <fstream>
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
    QSlider* slZoom;
protected:
    void paintEvent(QPaintEvent* event);
private:
    QWidget* spacer;
    QLabel* lGridSize;
    QLabel* lGridSize2;
    QLabel* lZoom;
    QActionGroup* qagMainToolbar;
    
    // Creates a QPixmap of a single given color
    inline QPixmap color2pixmap(QRgb color) {
        QPixmap result(12, 12);
        result.fill(color);
        return result;
    }

public slots:
    // Toolbar button slots
    inline void slot_gridsize(int gridsize) { view->renderer->setGridSize(gridsize); }
    inline void slot_zoom(int zoom) { view->model->zoom = zoom; }
    
    // Settings menu slots
    void slot_clampdist();
    void slot_pstdgridsize();
    
    // Scene menu slots
    void slot_clearscene();
    void slot_deleteselected();
    
    // View menu slots
    void slot_fpscounter();
    void slot_grid();
    inline void slot_showoutput() { view->simulator->toggle(); }
    
    // Operate menu slots
    inline void slot_selectdomain() { view->renderer->setState(SELECTDOMAIN); }
    inline void slot_select() { view->renderer->setState(SELECT); }
    inline void slot_move() { view->renderer->setState(MOVE); }
    inline void slot_adddomain() { view->renderer->setState(ADDDOMAIN); }
    inline void slot_addsource() { view->renderer->setState(ADDSOURCE); }
    inline void slot_addreceiver() { view->renderer->setState(ADDRECEIVER); }
    inline void slot_measure() { view->renderer->setState(MEASURE); }
    inline void slot_undo() { view->undo(); }
    inline void slot_redo() { view->redo(); }
    inline void slot_movetocenter() { view->renderer->moveToCenter(); }
    inline void slot_changeabsorption() { view->renderer->eh->changeabsorptiondialog(); }
    inline void slot_start() { view->simulator->start(); }
    
    // About menu slots
    void slot_changelog();
};

#endif