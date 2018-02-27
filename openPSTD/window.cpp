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
    
    // Create a settings instance
    settings = new Settings();
    
    // Create a GraphicsView for the main frame
    slZoom = new QSlider(Qt::Horizontal);
    view = new GraphicsView(this, settings, slZoom, ui->actionChangeAbsorption, ui->actionShow_Output);
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
    lGridSize->setToolTip("In m");
    ui->mainToolBar->addWidget(lGridSize);
    
    // Create a QSpinBox for the grid size
    sbGridSize = new QSpinBox();
    sbGridSize->setMinimum(settings->pstdGridSize);
    sbGridSize->setMaximum(10*settings->pstdGridSize);
    sbGridSize->setSingleStep(settings->pstdGridSize);
    sbGridSize->setValue(settings->pstdGridSize);
    sbGridSize->setToolTip("In m");
    view->renderer->setGridSize(settings->pstdGridSize);
    ui->mainToolBar->addWidget(sbGridSize);
    connect(sbGridSize, SIGNAL(valueChanged(int)), this, SLOT(slot_gridsize(int)));
    
    // Set initial scene offset
    view->model->offsetX = 0;
    view->model->offsetY = 0;
    view->model->state = SELECT;
    
    // Set action icons in settings menu
    ui->actionGrid_color->setIcon(QIcon(color2pixmap(settings->gridColor)));
    ui->actionBackground_color->setIcon(QIcon(color2pixmap(settings->bgColor)));
    ui->actionZoom_color->setIcon(QIcon(color2pixmap(settings->zoomColor)));
    ui->actionFPS_color->setIcon(QIcon(color2pixmap(settings->fpsColor)));
    ui->actionSource_color->setIcon(QIcon(color2pixmap(settings->sourceColor)));
    ui->actionReceiver_color->setIcon(QIcon(color2pixmap(settings->receiverColor)));
    
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
    qagMainToolbar->addAction(ui->actionAddSource);
    qagMainToolbar->addAction(ui->actionAddReceiver);
    qagMainToolbar->addAction(ui->actionMeasure);
    qagMainToolbar->setExclusive(true);
    
    // Connect actions in settings menu
    connect(ui->actionClamp_distance, SIGNAL(triggered(bool)), this, SLOT(slot_clampdist()));
    connect(ui->actionPSTD_grid_size, SIGNAL(triggered(bool)), this, SLOT(slot_pstdgridsize()));
    
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
    
    // Connect actions in the about menu
    connect(ui->actionChangelog, SIGNAL(triggered(bool)), this, SLOT(slot_changelog()));
    
    // Disable the change absorption action
    ui->actionChangeAbsorption->setEnabled(false);
}

/**
 * Destructor.
 */
Window::~Window() {
    // Delete class instance variables
    delete settings;
    delete qagMainToolbar;
    delete sbGridSize;
    delete view;
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
 * Callback method for the clamp distance action in the settings menu.
 * Opens an input dialog and saves the new clamp distance.
 */
void Window::slot_clampdist() {
    int clampdist = QInputDialog::getInt(
        this,
        "Choose a clamp distance",
        "Choose a clamp distance\nSet to zero to disable clamping",
        settings->clampDist
    );
    settings->clampDist = clampdist;
}

/**
 * Callback method for the pstd grid size action in the settings menu.
 * Opens an input dialog and saves the new pstd grid size.
 */
void Window::slot_pstdgridsize() {
    int pstdgridsize = QInputDialog::getInt(
        this,
        "Choose a PSTD grid size",
        "Choose a PSTD grid size",
        settings->pstdGridSize
    );
    settings->pstdGridSize = pstdgridsize;
    
    sbGridSize->setMinimum(settings->pstdGridSize);
    sbGridSize->setMaximum(10*settings->pstdGridSize);
    sbGridSize->setSingleStep(settings->pstdGridSize);
    sbGridSize->setValue(settings->pstdGridSize);
    view->renderer->setGridSize(settings->pstdGridSize);
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
    view->clearSelection();
}

/**
 * Callback method for the delete selected action in the scene menu.
 * Removes all selected objects from the model.
 */
void Window::slot_deleteselected() {
    // Delete all selected objects
    view->deleteSelected();
}

/**
 * Callback method for the FPS counter action in the view menu.
 * Toggles the display of the on-screen FPS counter.
 */
void Window::slot_fpscounter() {
    view->model->showFPS = ui->actionFPS_counter->isChecked();
    ui->actionFPS_color->setEnabled(view->model->showFPS);
}

/**
 * Callback method for the grid action in the view menu.
 * Toggles the display of the grid.
 */
void Window::slot_grid() {
    view->model->showGrid = ui->actionGrid->isChecked();
}

/**
 * Callback method for the changelog action in the about menu.
 * Shows a popup containing the changelog of the current version.
 */
void Window::slot_changelog() {
    // Read the changelog file
    std::string changelog = "";
    std::ifstream infile("../changelog");
    std::string line;
    while (std::getline(infile, line)) {
        if (changelog != "") changelog += "\n";
        changelog += line;
    }
    
    // Show a message box with the changelog text
    QMessageBox box(
        "Changelog",
        QString(changelog.c_str()),
        QMessageBox::NoIcon,
        QMessageBox::Close,
        QMessageBox::NoButton,
        QMessageBox::NoButton
    );
    box.exec();
}