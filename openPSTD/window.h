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
#include "ui_window.h"
#include "settings.h"
#include "graphicsview.h"
#include "modelmanager.h"

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
    Renderer* renderer;
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
    EventListener* eventlistener;
    
    // Creates a QPixmap of a single given color
    inline QPixmap color2pixmap(QRgb color) {
        QPixmap result(12, 12);
        result.fill(color);
        return result;
    }

public slots:
    // Toolbar button slots
    inline void slot_gridsize(int gridsize) { /* TODO */ }
    inline void slot_zoom(int zoom) { /* TOTO */ }
    
    // Settings menu slots
    void slot_gridcolor();
    void slot_bgcolor();
    void slot_zoomcolor();
    void slot_fpscolor();
    void slot_sourcecolor();
    void slot_receivercolor();
    void slot_clampdist();
    void slot_pstdgridsize();
    
    // Scene menu slots
    void slot_clearscene();
    void slot_deleteselected();
    
    // View menu slots
    void slot_fpscounter();
    void slot_grid();
    
    // Operate menu slots
    inline void slot_select() { eventlistener->action(A_SELECT); }
    inline void slot_move() { eventlistener->action(A_MOVESCENE); }
    inline void slot_adddomain() { eventlistener->action(A_ADDDOMAIN); }
    inline void slot_addsource() { eventlistener->action(A_ADDSOURCE); }
    inline void slot_addreceiver() { eventlistener->action(A_ADDRECEIVER); }
    inline void slot_measure() { eventlistener->action(A_MEASURE); }
    inline void slot_undo() { eventlistener->action(A_UNDO); }
    inline void slot_redo() { eventlistener->action(A_REDO); }
    inline void slot_movetocenter() { eventlistener->action(A_CENTERSCENE); }
    
    // About menu slots
    void slot_changelog();
};

#endif