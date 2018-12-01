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
#include <QFileDialog>
#include <QMessageBox>
#include <fstream>
#include "renderer.h"
#include "graphicsview.h"
#include "ui_window.h"
#include "coordinatedialog.h"
#include "simulator2.h"
#include "sidebar.h"

namespace Ui {
class Window;
}

class Window : public QMainWindow {
	Q_OBJECT
public:
	explicit Window(QWidget* parent = nullptr);
	Ui::Window* ui;
	GraphicsView* view;
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
	QAction* actionZoomMinus;
	QAction* actionZoomPlus;
	QActionGroup* qagMainToolbar;
	Simulator2* simulator;
	Sidebar* sidebar;
	
	// Creates a QPixmap of a single given color
	inline QPixmap color2pixmap(QRgb color) {
		QPixmap result(12, 12);
		result.fill(color);
		return result;
	}
public slots:
	// Toolbar button slots
	inline void slot_gridsize(int gridsize) { view->renderer->setGridSize(gridsize); }
	inline void slot_zoom(int zoom) { ModelManager::getInstance()->getCurrent()->zoom = zoom; }
	inline void slot_zoomminus() { ModelManager::getInstance()->getCurrent()->zoom--; }
	inline void slot_zoomplus() { ModelManager::getInstance()->getCurrent()->zoom++; }
	
	// File menu slots
	void slot_exporttocsv();
	void slot_close();
	
	// Settings menu slots
	void slot_clampdist();
	void slot_guigridsize();
	void slot_pstdgridsize();
	void slot_windowsize();
	void slot_rendertime();
	void slot_pmlcells();
	void slot_attenuationpmlcells();
	void slot_airdensity();
	void slot_soundspeed();
	
	// Scene menu slots
	void slot_clearscene();
	void slot_deleteselected();
	
	// View menu slots
	void slot_fpscounter();
	void slot_grid();
	inline void slot_showoutput() { simulator->toggle(); }
	inline void slot_showsidebar() { sidebar->updateVisibility(); }
	
	// Operate menu slots
	inline void slot_selectdomain() { view->renderer->setState(SELECTDOMAIN); }
	inline void slot_select() { view->renderer->setState(SELECT); }
	inline void slot_move() { view->renderer->setState(MOVE); }
	inline void slot_adddomain() { view->renderer->setState(ADDDOMAIN); }
	inline void slot_addsource() { view->renderer->eh->addSource(); }
	inline void slot_addreceiver() { view->renderer->eh->addReceiver(); }
	inline void slot_measure() { view->renderer->setState(MEASURE); }
	inline void slot_undo() { ModelManager::getInstance()->undo(); }
	inline void slot_redo() { ModelManager::getInstance()->redo(); }
	inline void slot_movetocenter() { view->renderer->eh->moveToCenter(); }
	inline void slot_changeabsorption() { view->renderer->eh->changeAbsorption(); }
	inline void slot_start() { simulator->start(); }
	inline void slot_stop() { simulator->stop(); }
    inline void slot_removesimulation() { simulator->removeSimulation(); }
};

#endif