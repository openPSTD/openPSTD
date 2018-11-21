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
	Model* model = ModelManager::getInstance()->getCurrent();
	model->actionSelect_Domain = ui->actionSelect_Domain;
	model->actionSelect = ui->actionSelect;
	model->actionMoveScene = ui->actionMoveScene;
	model->actionAddDomain = ui->actionAddDomain;
	model->actionAddSource = ui->actionAddSource;
	model->actionAddReceiver = ui->actionAddReceiver;
	model->actionMeasure = ui->actionMeasure;
	model->actionUndo = ui->actionUndo;
	model->actionRedo = ui->actionRedo;
	model->actionMoveToCenter = ui->actionMoveToCenter;
	model->actionChangeAbsorption = ui->actionChangeAbsorption;
	model->actionShow_Output = ui->actionShow_Output;
	model->actionShowSidebar = ui->actionSettings_Sidebar;
	model->actionStart = ui->actionstart;
	model->actionStop = ui->actionstop;
	model->actionRemoveSimulation = ui->actionRemove_Simulation;
	
	// Disable the stop simulator action by default
	ui->actionstop->setEnabled(false);
	ui->actionRemove_Simulation->setEnabled(false);
	
	// Add a spacer to the main toolbar
	spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->mainToolBar->addWidget(spacer);
	
	// Create a Simulator instance
    sidebar = new Sidebar();
	simulator = new Simulator2(this, ui->statusBar, sidebar);
	
	// Create a GraphicsView for the main frame
	slZoom = new QSlider(Qt::Horizontal);
	view = new GraphicsView(this, slZoom, simulator);
	view->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	ui->horizontalLayout->addWidget(view);
	
	// Add the sidebar
	ui->horizontalLayout->addWidget(sidebar);
	connect(ui->actionSettings_Sidebar, SIGNAL(triggered()), this, SLOT(slot_showsidebar()));
	
	// Set initial model variables
	ModelManager::getInstance()->getCurrent()->showFPS = ui->actionFPS_counter->isChecked();
	
	// Add a label for the zoom level spinbox
	lZoom = new QLabel();
	lZoom->setText("Zoom level:");
	lZoom->setToolTip("In pixels / m");
	ui->mainToolBar->addWidget(lZoom);
	
	// Create a QSpinBox for the zoom level
	slZoom->setMinimum(1);
	slZoom->setMaximum(100);
	slZoom->setValue(10);
	slZoom->setFixedWidth(100);
	ModelManager::getInstance()->getCurrent()->zoom = 10;
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
	sbGridSize->setMinimum(4);
	sbGridSize->setValue(settings->guiGridSize);
	sbGridSize->setToolTip("In m");
	view->renderer->setGridSize(settings->guiGridSize);
	ui->mainToolBar->addWidget(sbGridSize);
	connect(sbGridSize, SIGNAL(valueChanged(int)), this, SLOT(slot_gridsize(int)));
	
	// Add a "(m)" label after the grid size QSpinBox
	lGridSize2 = new QLabel();
	lGridSize2->setText("(m)");
	ui->mainToolBar->addWidget(lGridSize2);
	
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
    connect(ui->actionRemove_Simulation, SIGNAL(triggered(bool)), this, SLOT(slot_removesimulation()));
	
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
	delete sidebar;
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
    Model* model = ModelManager::getInstance()->getCurrent();
	if (!model->hasSimulationOutput || model->simulating) {
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
	std::vector<Frame> frames = simulator->getFrames();
    
	// Loop through all receivers
	for (unsigned int i = 0; i < model->receivers.size(); i++) {
		// Get the position of this receiver
		int x = model->receivers[i]->getX();
		int y = model->receivers[i]->getY();
		
		// Open a file stream for this file
		std::ofstream outfile(dir + "/receiver_" + std::to_string(x) + "_" + std::to_string(y) + ".csv");
		
		// Compute which domain the receiver is in
		unsigned int domainID;
        int sx, sy;
		for (unsigned int j = 0; j < model->domains.size(); j++) {
			// Get the position of this domain
			int x0 = model->domains[j]->getMinX();
			int x1 = model->domains[j]->getMaxX();
			int y0 = model->domains[j]->getMinY();
			int y1 = model->domains[j]->getMaxY();
			
			// Check if the receiver is inside this domain
			bool xmatch = x0 <= x && x <= x1;
			bool ymatch = y0 <= y && y <= y1;
			if (xmatch && ymatch) {
                // Save the domainID
				domainID = j;
                
                // Compute the sample coordinates
                unsigned int w = frames[0].getWidth(domainID);
                unsigned int h = frames[0].getHeight(domainID);
                sx = ((x-x0)*static_cast<int>(w)/(x1-x0));
                sy = ((y-y0)*static_cast<int>(h)/(y1-y0));
			}
		}
        
        // Loop through all frames
        for (unsigned int frameID = 0; frameID < frames.size(); frameID++) {
            // Get the pressure value at the receiver position
            double p = frames[frameID].getSample(sx, sy, domainID);
            outfile << p << ",";
        }
        
        // Close the output file
        outfile.close();
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
	Model* model = ModelManager::getInstance()->getCurrent();
	double pstdgridspacing = QInputDialog::getDouble(
		this,
		"Choose a PSTD grid spacing",
		"Choose a PSTD grid spacing",
		model->gridspacing
	);
	model->gridspacing = pstdgridspacing;
}

void Window::slot_windowsize() {
	// TODO: Window size has to be 16 or 32
	Model* model = ModelManager::getInstance()->getCurrent();
	int windowsize = QInputDialog::getInt(
		this,
		"Choose a window size",
		"Choose a window size",
		model->windowsize
	);
	model->windowsize = windowsize;
}

void Window::slot_rendertime() {
	Model* model = ModelManager::getInstance()->getCurrent();
	double rendertime = QInputDialog::getDouble(
		this,
		"Choose a render time",
		"Choose a render time",
		model->rendertime
	);
	model->rendertime = rendertime;
}

void Window::slot_pmlcells() {
	Model* model = ModelManager::getInstance()->getCurrent();
	int pmlcells = QInputDialog::getInt(
		this,
		"Choose a number of PML cells",
		"Choose a number of PML cells",
		model->pmlcells
	);
	model->pmlcells = pmlcells;
}

void Window::slot_attenuationpmlcells() {
	Model* model = ModelManager::getInstance()->getCurrent();
	int attenuationpmlcells = QInputDialog::getInt(
		this,
		"Choose an attenuation of PML cells",
		"Choose an attenuation of PML cells",
		model->attenuationpmlcells
	);
	model->attenuationpmlcells = attenuationpmlcells;
}

void Window::slot_airdensity() {
	Model* model = ModelManager::getInstance()->getCurrent();
	double airdensity = QInputDialog::getDouble(
		this,
		"Choose an air density",
		"Choose an air density",
		model->airdensity
	);
	model->airdensity = airdensity;
}

void Window::slot_soundspeed() {
	Model* model = ModelManager::getInstance()->getCurrent();
	int soundspeed = QInputDialog::getInt(
		this,
		"Choose a sound speed",
		"Choose a sound speed",
		model->soundspeed
	);
	model->soundspeed = soundspeed;
}

/**
 * Callback method for the clear scene action in the scene menu.
 * Removes all domains, sources, and receivers from the model.
 */
void Window::slot_clearscene() {
	// Delete all domains
	ModelManager::getInstance()->saveState();
	Model* model = ModelManager::getInstance()->getCurrent();
	model->domains.clear();
	
	// Delete all sources
	model->sources.clear();
	
	// Delete all receivers
	model->receivers.clear();
	
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
	ModelManager::getInstance()->getCurrent()->showFPS = ui->actionFPS_counter->isChecked();
}

/**
 * Callback method for the grid action in the view menu.
 * Toggles the display of the grid.
 */
void Window::slot_grid() {
	ModelManager::getInstance()->getCurrent()->showGrid = ui->actionGrid->isChecked();
}
