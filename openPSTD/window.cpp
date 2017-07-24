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
    view = new GraphicsView(this, settings);
    view->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->horizontalLayout->addWidget(view);
    
    // Set initial model variables
    view->model->showFPS = ui->actionFPS_counter->isChecked();
    
    // Add a label for the zoom level spinbox
    lZoom = new QLabel();
    lZoom->setText("Zoom level:");
    lZoom->setToolTip("In mm / pixel");
    ui->mainToolBar->addWidget(lZoom);
    
    // Create a QSpinBox for the zoom level
    sbZoom = new QSpinBox();
    sbZoom->setMinimum(5);
    sbZoom->setMaximum(50);
    sbZoom->setValue(5);
    sbZoom->setToolTip("In mm / pixel");
    view->model->zoom = 5;
    ui->mainToolBar->addWidget(sbZoom);
    connect(sbZoom, SIGNAL(valueChanged(int)), this, SLOT(slot_zoom(int)));
    
    QWidget* w = new QWidget();
    w->setFixedWidth(20);
    ui->mainToolBar->addWidget(w);
    
    // Add a label for the grid size spinbox
    lGridSize = new QLabel();
    lGridSize->setText("Grid size:");
    lGridSize->setToolTip("In mm");
    ui->mainToolBar->addWidget(lGridSize);
    
    // Create a QSpinBox for the grid size
    sbGridSize = new QSpinBox();
    sbGridSize->setMinimum(5);
    sbGridSize->setMaximum(50);
    sbGridSize->setValue(25);
    sbGridSize->setToolTip("In mm");
    view->renderer->setGridSize(25);
    view->model->zoom = 5;
    ui->mainToolBar->addWidget(sbGridSize);
    connect(sbGridSize, SIGNAL(valueChanged(int)), this, SLOT(slot_gridsize(int)));
    
    // Center main window on screen
    QDesktopWidget* desktop = QApplication::desktop();
    QRect screen = desktop->availableGeometry(desktop->primaryScreen());
    int x = screen.x() + (screen.width() - 1024) / 2;
    int y = screen.y() + (screen.height() - 768) / 2;
    this->move(x, y);
    
    // Create a QActionGroup for the toolbar buttons
    qagMainToolbar = new QActionGroup(parent);
    qagMainToolbar->addAction(ui->actionSelect);
    qagMainToolbar->addAction(ui->actionMoveScene);
    qagMainToolbar->addAction(ui->actionAddDomain);
    qagMainToolbar->addAction(ui->actionAddSource);
    qagMainToolbar->addAction(ui->actionAddReceiver);
    qagMainToolbar->setExclusive(true);
    
    // Connect actions for menu buttons
    connect(ui->actionSelect, SIGNAL(triggered(bool)), this, SLOT(slot_select()));
    connect(ui->actionMoveScene, SIGNAL(triggered(bool)), this, SLOT(slot_move()));
    connect(ui->actionAddDomain, SIGNAL(triggered(bool)), this, SLOT(slot_adddomain()));
    connect(ui->actionAddSource, SIGNAL(triggered(bool)), this, SLOT(slot_addsource()));
    connect(ui->actionAddReceiver, SIGNAL(triggered(bool)), this, SLOT(slot_addreceiver()));
    
    // Connect actions in settings menu
    connect(ui->actionGrid_color, SIGNAL(triggered(bool)), this, SLOT(slot_gridcolor()));
    connect(ui->actionBackground_color, SIGNAL(triggered(bool)), this, SLOT(slot_bgcolor()));
    connect(ui->actionZoom_color, SIGNAL(triggered(bool)), this, SLOT(slot_zoomcolor()));
    connect(ui->actionFPS_color, SIGNAL(triggered(bool)), this, SLOT(slot_fpscolor()));
    
    // Connect actions in view menu
    connect(ui->actionFPS_counter, SIGNAL(triggered(bool)), this, SLOT(slot_fpscounter()));
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
 * Callback method for the grid color action in the settings menu.
 * Opens a color picker dialog and saves the new color.
 */
void Window::slot_gridcolor() {
    QColor color = QColorDialog::getColor(settings->gridColor, this, "Choose a grid color");
    if (color.isValid()) settings->gridColor = color.rgb();
}

/**
 * Callback method for the background color action in the settings menu.
 * Opens a color picker dialog and saves the new color.
 */
void Window::slot_bgcolor() {
    QColor color = QColorDialog::getColor(settings->bgColor, this, "Choose a background color");
    if (color.isValid()) settings->bgColor = color.rgb();
}

/**
 * Callback method for the zoom color action in the settings menu.
 * Opens a color picker dialog and saves the new color.
 */
void Window::slot_zoomcolor() {
    QColor color = QColorDialog::getColor(settings->zoomColor, this, "Choose a zoom color");
    if (color.isValid()) settings->zoomColor = color.rgb();
}

/**
 * Callback method for the fps color action in the settings menu.
 * Opens a color picker dialog and saves the new color.
 */
void Window::slot_fpscolor() {
    QColor color = QColorDialog::getColor(settings->fpsColor, this, "Choose an FPS color");
    if (color.isValid()) settings->fpsColor = color.rgb();
}