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
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolBar->addWidget(spacer);
    
    // Add a label for the grid size spinbox
    QLabel* lGridSize = new QLabel();
    lGridSize->setText("Grid size:");
    ui->mainToolBar->addWidget(lGridSize);
    
    // Create a GraphicsView for the main frame
    view = new GraphicsView(this);
    view->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->horizontalLayout->addWidget(view);
    
    // Set initial model variables
    view->model->showFPS = ui->actionFPS_counter->isChecked();
    
    // Create a QSpinBox for the grid size
    sbGridSize = new QSpinBox();
    sbGridSize->setMinimum(5);
    sbGridSize->setMaximum(50);
    sbGridSize->setValue(25);
    view->renderer->setGridSize(25);
    ui->mainToolBar->addWidget(sbGridSize);
    connect(sbGridSize, SIGNAL(valueChanged(int)), this, SLOT(slot_gridsize(int)));
    
    // Center main window on screen
    QDesktopWidget* desktop = QApplication::desktop();
    QRect screen = desktop->availableGeometry(desktop->primaryScreen());
    int x = screen.x() + (screen.width() - 1024) / 2;
    int y = screen.y() + (screen.height() - 768) / 2;
    this->move(x, y);
    
    // Create a QActionGroup for the toolbar buttons
    QActionGroup* qagMainToolbar = new QActionGroup(parent);
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
    
    // Connect actions in view menu
    connect(ui->actionFPS_counter, SIGNAL(triggered(bool)), this, SLOT(slot_fpscounter()));
}

/**
 * Destructor.
 */
Window::~Window() {
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