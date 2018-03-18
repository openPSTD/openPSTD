#include "coordinatedialog.h"
#include "ui_coordinatedialog.h"

/**
 * Constructor.
 * 
 * @param parent  A reference to the main window
 */
CoordinateDialog::CoordinateDialog(QWidget *parent, QPoint initial) : QDialog(parent), ui(new Ui::CoordinateDialog) {
    // Set the initial values of the output variables
    x = initial.x();
    y = initial.y();
    
    // Setup the base UI
    ui->setupUi(this);
    
    // Update the values of the spinboxes
    ui->sbX->setValue(x);
    ui->sbY->setValue(y);
    
    // Connect actions to the save and cancel buttons
    connect(ui->buttonCancel, SIGNAL(clicked(bool)), this, SLOT(slot_cancel()));
    connect(ui->buttonSave, SIGNAL(clicked(bool)), this, SLOT(slot_save()));
    
    // Set the window title
    setWindowTitle("Enter coordinates");
    
    // Focus the x-coordinate spinbox
    ui->sbX->setFocus();
}

/**
 * Destructor.
 */
CoordinateDialog::~CoordinateDialog() {
    delete ui;
}

/**
 * Callback method for when the cancel button is pressed.
 */
void CoordinateDialog::slot_cancel() {
    // Close the window without saving
    close();
}

/**
 * Callback method for when the save button is pressed.
 */
void CoordinateDialog::slot_save() {
    // Save the coordinates into the output variables
    x = ui->sbX->value();
    y = ui->sbY->value();
    
    // Close the window
    close();
}