#include "absorptiondialog.h"
#include "ui_absorptiondialog.h"

/**
 * Constructor.
 * 
 * @param parent  A reference to the main window
 * @param top  The initial absorption coefficient of the top wall
 * @param bottom  The initial absorption coefficient of the bottom wall
 * @param left  The initial absorption coefficient of the left wall
 * @param right  The initial absorption coefficient of the right wall
 */
AbsorptionDialog::AbsorptionDialog(QWidget* parent, double top, double bottom, double left, double right) : QDialog(parent), ui(new Ui::AbsorptionDialog) {
    // Save the initial absorption values
    this->top = top;
    this->bottom = bottom;
    this->left = left;
    this->right = right;
    
    // Setup the base UI
    ui->setupUi(this);
    
    // Set current absorption values in the spinboxes
    ui->spinBoxTop->setValue(top);
    ui->spinBoxBottom->setValue(bottom);
    ui->spinBoxLeft->setValue(left);
    ui->spinBoxRight->setValue(right);
    
    // Connect actions to the save and cancel buttons
    connect(ui->buttonCancel, SIGNAL(clicked(bool)), this, SLOT(slot_cancel()));
    connect(ui->buttonSave, SIGNAL(clicked(bool)), this, SLOT(slot_save()));
    
    // Set the window title
    setWindowTitle("Change absorption coefficients");
}

/**
 * Destructor.
 */
AbsorptionDialog::~AbsorptionDialog() {
    // Delete the UI
    delete ui;
}

/**
 * Callback method for when the cancel button is pressed.
 */
void AbsorptionDialog::slot_cancel() {
    // Close the window without saving
    close();
}

/**
 * Callback method for when the save button is pressed.
 */
void AbsorptionDialog::slot_save() {
    // Save the new absorption values into the output variables
    top = ui->spinBoxTop->value();
    bottom = ui->spinBoxBottom->value();
    left = ui->spinBoxLeft->value();
    right = ui->spinBoxRight->value();
    
    // Close the window
    close();
}