#include "window.h"

/**
 * Constructor.
 * Initializes the main window.
 * 
 * @param parent  A pointer to the parent widget
 */
Window::Window(QWidget* parent) : QMainWindow(parent), ui(new Ui::Window) {
	// Setup basic UI
	ui->setupUi(this);
	
	// Add a spacer to the main toolbar
	spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->mainToolBar->addWidget(spacer);
	
	// Create a GraphicsView for the main frame
	slZoom = new QSlider(Qt::Horizontal);
	view = new GraphicsView(this, slZoom, ui->actionChangeAbsorption, ui->actionShow_Output, ui->statusBar);
	view->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	ui->horizontalLayout->addWidget(view);
	
	// Set initial model variables
	view->model->showFPS = ui->actionFPS_counter->isChecked();
	
	// Add a label for the zoom level spinbox
	lZoom = new QLabel();
	lZoom->setText("Zoom level:");
	lZoom->setToolTip("In pixels / m");
	ui->mainToolBar->addWidget(lZoom);
	
	// Create a QSpinBox for the zoom level
	slZoom->setMinimum(1);
	slZoom->setMaximum(100);
	slZoom->setValue(5);
	slZoom->setFixedWidth(100);
	view->model->zoom = 5;
	ui->mainToolBar->addWidget(slZoom);
	connect(slZoom, SIGNAL(valueChanged(int)), this, SLOT(slot_zoom(int)));
	
	QWidget* w = new QWidget();
	w->setFixedWidth(20);
	ui->mainToolBar->addWidget(w);
	
	// Add a label for the grid size spinbox
	lGridSize = new QLabel();
	lGridSize->setText("Grid size:");
	ui->mainToolBar->addWidget(lGridSize);
	
	// Create a QSpinBox for the grid size
	Settings* settings = Settings::getInstance();
	sbGridSize = new QSpinBox();
	sbGridSize->setMinimum(settings->guiGridSize);
	sbGridSize->setMaximum(10*settings->guiGridSize);
	sbGridSize->setSingleStep(settings->guiGridSize);
	sbGridSize->setValue(settings->guiGridSize);
	sbGridSize->setToolTip("In m");
	view->renderer->setGridSize(settings->guiGridSize);
	ui->mainToolBar->addWidget(sbGridSize);
	connect(sbGridSize, SIGNAL(valueChanged(int)), this, SLOT(slot_gridsize(int)));
	
	// Add a "(m)" label after the grid size QSpinBox
	lGridSize2 = new QLabel();
	lGridSize2->setText("(m)");
	ui->mainToolBar->addWidget(lGridSize2);
	
	// Set initial scene offset
	view->model->state = SELECT;
	
	// Center main window on screen
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screen = desktop->availableGeometry(desktop->primaryScreen());
	int x = screen.x() + (screen.width() - 1024) / 2;
	int y = screen.y() + (screen.height() - 768) / 2;
	this->move(x, y);
	
	// Create a QActionGroup for the toolbar buttons
	qagMainToolbar = new QActionGroup(parent);
	qagMainToolbar->addAction(ui->actionSelect_Domain);
	qagMainToolbar->addAction(ui->actionSelect);
	qagMainToolbar->addAction(ui->actionMoveScene);
	qagMainToolbar->addAction(ui->actionAddDomain);
	qagMainToolbar->addAction(ui->actionMeasure);
	qagMainToolbar->setExclusive(true);
	
	// Connect actions in file menu
	connect(ui->actionExport_to_CSV, SIGNAL(triggered(bool)), this, SLOT(slot_exporttocsv()));
	connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(slot_close()));
	
	// Connect actions in settings menu
	connect(ui->actionClamp_distance, SIGNAL(triggered(bool)), this, SLOT(slot_clampdist()));
	connect(ui->actionGUI_grid_size, SIGNAL(triggered(bool)), this, SLOT(slot_guigridsize()));
	connect(ui->actionPSTD_grid_size, SIGNAL(triggered(bool)), this, SLOT(slot_pstdgridsize()));
	connect(ui->actionWindow_size, SIGNAL(triggered(bool)), this, SLOT(slot_windowsize()));
	connect(ui->actionRender_time, SIGNAL(triggered(bool)), this, SLOT(slot_rendertime()));
	connect(ui->actionPML_Cells, SIGNAL(triggered(bool)), this, SLOT(slot_pmlcells()));
	connect(ui->actionAttenuation_of_PML_cells, SIGNAL(triggered(bool)), this, SLOT(slot_attenuationpmlcells()));
	connect(ui->actionAir_density, SIGNAL(triggered(bool)), this, SLOT(slot_airdensity()));
	connect(ui->actionSound_speed, SIGNAL(triggered(bool)), this, SLOT(slot_soundspeed()));
	
	// Connect actions in scene menu
	connect(ui->actionClear_all, SIGNAL(triggered(bool)), this, SLOT(slot_clearscene()));
	connect(ui->actionDelete_selected, SIGNAL(triggered(bool)), this, SLOT(slot_deleteselected()));
	
	// Connect actions in view menu
	connect(ui->actionFPS_counter, SIGNAL(triggered(bool)), this, SLOT(slot_fpscounter()));
	connect(ui->actionGrid, SIGNAL(triggered(bool)), this, SLOT(slot_grid()));
	connect(ui->actionShow_Output, SIGNAL(triggered(bool)), this, SLOT(slot_showoutput()));
	
	// Connect actions in operate menu
	connect(ui->actionSelect_Domain, SIGNAL(triggered(bool)), this, SLOT(slot_selectdomain()));
	connect(ui->actionSelect, SIGNAL(triggered(bool)), this, SLOT(slot_select()));
	connect(ui->actionMoveScene, SIGNAL(triggered(bool)), this, SLOT(slot_move()));
	connect(ui->actionAddDomain, SIGNAL(triggered(bool)), this, SLOT(slot_adddomain()));
	connect(ui->actionAddSource, SIGNAL(triggered(bool)), this, SLOT(slot_addsource()));
	connect(ui->actionAddReceiver, SIGNAL(triggered(bool)), this, SLOT(slot_addreceiver()));
	connect(ui->actionMeasure, SIGNAL(triggered(bool)), this, SLOT(slot_measure()));
	connect(ui->actionUndo, SIGNAL(triggered(bool)), this, SLOT(slot_undo()));
	connect(ui->actionRedo, SIGNAL(triggered(bool)), this, SLOT(slot_redo()));
	connect(ui->actionMoveToCenter, SIGNAL(triggered(bool)), this, SLOT(slot_movetocenter()));
	connect(ui->actionChangeAbsorption, SIGNAL(triggered(bool)), this, SLOT(slot_changeabsorption()));
	connect(ui->actionstart, SIGNAL(triggered(bool)), this, SLOT(slot_start()));
	connect(ui->actionstop, SIGNAL(triggered(bool)), this, SLOT(slot_stop()));
	
	// Disable the change absorption action
	ui->actionChangeAbsorption->setEnabled(false);
	ui->statusBar->showMessage("Status: Ready");
}

/**
 * Destructor.
 */
Window::~Window() {
	// Delete class instance variables
	Settings::getInstance()->destruct();
	delete qagMainToolbar;
	delete sbGridSize;
	delete view;
	delete lGridSize2;
	delete lGridSize;
	delete spacer;
	delete ui;
}

/**
 * PaintEvent method, called when the window needs to be repainted.
 * 
 * @param event  A reference to the QPaintEvent
 */
void Window::paintEvent(QPaintEvent* event) {
	QMainWindow::paintEvent(event);
	view->renderer->draw();
}

/**
 * Callback method for the exporttocsv action in the settings menu.
 * Exports the simulation output to csv files.
 */
void Window::slot_exporttocsv() {
	// Verify that a simulation has been completed
	if (!view->simulator->getCompleted()) {
		QMessageBox::information(
			this,
			"Export to csv",
			"Cannot export the simulation results, simulation has not completed yet."
		);
		return;
	}
	
	// Get an output directory
	std::string dir = QFileDialog::getExistingDirectory(
		this,
		"Select output directory",
		"."
	).toStdString();
	
	// Get the frame data from Simulator
	std::vector<Frame> frames = view->renderer->simulator->getFrames();
	
	// Loop through all receivers
	for (unsigned int i = 0; i < view->model->receivers.size(); i++) {
		// Get the position of this receiver
		int x = view->model->receivers[i].getX();
		int y = view->model->receivers[i].getY();
		
		// Open a file stream for this file
		std::ofstream outfile(dir + "/receiver_" + std::to_string(x) + "_" + std::to_string(-y) + ".csv");
		
		// Compute which domain the receiver is in
		unsigned int domainID;
		for (unsigned int j = 0; j < view->model->domains.size(); j++) {
			// Get the position of this domain
			int dx0 = view->model->domains[j].getX0();
			int dx1 = view->model->domains[j].getX1();
			int dy0 = view->model->domains[j].getY0();
			int dy1 = view->model->domains[j].getY1();
			
			// Check if the receiver is inside this domain
			bool xm = dx0 <= x && x <= dx1;
			bool ym = dy0 <= y && y <= dy1;
			if (xm && ym) {
				domainID = j;
				break;
			}
		}
		
		// Compute the position of the receiver in kernel coordinates
		int px = view->model->receivers[i].getX() * view->model->zoom + view->model->offsetX;
		int py = view->model->receivers[i].getY() * view->model->zoom + view->model->offsetY;
		int x0 = view->model->domains[domainID].getX0();
		int x1 = view->model->domains[domainID].getX1();
		int y0 = view->model->domains[domainID].getY0();
		int y1 = view->model->domains[domainID].getY1();
		int minx = x0 * view->model->zoom + view->model->offsetX;
		int maxx = x1 * view->model->zoom + view->model->offsetX;
		int miny = y0 * view->model->zoom + view->model->offsetY;
		int maxy = y1 * view->model->zoom + view->model->offsetY;
		unsigned int fwidth = frames[0].getWidth(domainID);
		unsigned int fheight = frames[0].getHeight(domainID);
		int ix = (px - minx) * static_cast<int>(fwidth-1) / (maxx - minx) + 1;
		int iy = (maxy - py) * static_cast<int>(fheight-1) / (maxy - miny) + 1;
		
		// Loop through all frames
		for (unsigned int j = 0; j < frames.size(); j++) {
			// Get the pressure of the receiver at this frame
			double pressure = frames[j].getSample(
				static_cast<unsigned int>(ix),
				static_cast<unsigned int>(iy),
				domainID
			);
			if (j != 0) outfile << ",";
			outfile << pressure;
		}
		outfile.close();
		std::cout << "Saved receiver " << i << std::endl;
	}
}

/**
 * Callback method for the close action in the settings menu.
 * Closes the application.
 */
void Window::slot_close() {
	QApplication::quit();
}

/**
 * Callback method for the clamp distance action in the settings menu.
 * Opens an input dialog and saves the new clamp distance.
 */
void Window::slot_clampdist() {
	int clampdist = QInputDialog::getInt(
		this,
		"Choose a clamp distance",
		"Choose a clamp distance\nSet to zero to disable clamping",
		Settings::getInstance()->clampDist
	);
	Settings::getInstance()->clampDist = clampdist;
}

/**
 * Callback method for the gui grid size action in the settings menu.
 * Opens an input dialog and saves the new pstd grid size.
 */
void Window::slot_guigridsize() {
	Settings* settings = Settings::getInstance();
	int guigridsize = QInputDialog::getInt(
		this,
		"Choose a GUI grid size",
		"Choose a GUI grid size",
		settings->guiGridSize
	);
	settings->guiGridSize = guigridsize;
	
	sbGridSize->setMinimum(settings->guiGridSize);
	sbGridSize->setMaximum(10*settings->guiGridSize);
	sbGridSize->setSingleStep(settings->guiGridSize);
	sbGridSize->setValue(settings->guiGridSize);
	view->renderer->setGridSize(settings->guiGridSize);
}

void Window::slot_pstdgridsize() {
	Settings* settings = Settings::getInstance();
	double pstdgridsize = QInputDialog::getDouble(
		this,
		"Choose a PSTD grid size",
		"Choose a PSTD grid size",
		settings->pstdGridSize
	);
	settings->pstdGridSize = pstdgridsize;
}

void Window::slot_windowsize() {
	// TODO: Window size has to be 16 or 32
	Settings* settings = Settings::getInstance();
	int windowsize = QInputDialog::getInt(
		this,
		"Choose a window size",
		"Choose a window size",
		settings->windowSize
	);
	settings->windowSize = windowsize;
}

void Window::slot_rendertime() {
	Settings* settings = Settings::getInstance();
	double rendertime = QInputDialog::getDouble(
		this,
		"Choose a render time",
		"Choose a render time",
		settings->renderTime
	);
	settings->renderTime = rendertime;
}

void Window::slot_pmlcells() {
	Settings* settings = Settings::getInstance();
	int pmlcells = QInputDialog::getInt(
		this,
		"Choose a number of PML cells",
		"Choose a number of PML cells",
		settings->pmlcells
	);
	settings->pmlcells = pmlcells;
}

void Window::slot_attenuationpmlcells() {
	Settings* settings = Settings::getInstance();
	int attenuationpmlcells = QInputDialog::getInt(
		this,
		"Choose an attenuation of PML cells",
		"Choose an attenuation of PML cells",
		settings->attenuationpmlcells
	);
	settings->attenuationpmlcells = attenuationpmlcells;
}

void Window::slot_airdensity() {
	Settings* settings = Settings::getInstance();
	double airdensity = QInputDialog::getDouble(
		this,
		"Choose an air density",
		"Choose an air density",
		settings->airDensity
	);
	settings->airDensity = airdensity;
}

void Window::slot_soundspeed() {
	Settings* settings = Settings::getInstance();
	int soundspeed = QInputDialog::getInt(
		this,
		"Choose a sound speed",
		"Choose a sound speed",
		settings->soundSpeed
	);
	settings->soundSpeed = soundspeed;
}

/**
 * Callback method for the clear scene action in the scene menu.
 * Removes all domains, sources, and receivers from the model.
 */
void Window::slot_clearscene() {
	// Delete all domains
	view->model->domains.clear();
	
	// Delete all sources
	view->model->sources.clear();
	
	// Delete all receivers
	view->model->receivers.clear();
	
	// Clear the selected objects vector
	view->renderer->eh->clearSelection();
}

/**
 * Callback method for the delete selected action in the scene menu.
 * Removes all selected objects from the model.
 */
void Window::slot_deleteselected() {
	// Delete all selected objects
	view->renderer->eh->deleteSelected();
}

/**
 * Callback method for the FPS counter action in the view menu.
 * Toggles the display of the on-screen FPS counter.
 */
void Window::slot_fpscounter() {
	view->model->showFPS = ui->actionFPS_counter->isChecked();
}

/**
 * Callback method for the grid action in the view menu.
 * Toggles the display of the grid.
 */
void Window::slot_grid() {
	view->model->showGrid = ui->actionGrid->isChecked();
}
