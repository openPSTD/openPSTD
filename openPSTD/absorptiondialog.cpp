#include "absorptiondialog.h"
#include "ui_absorptiondialog.h"

AbsorptionDialog::AbsorptionDialog(QWidget* parent, unsigned int domain, Model* model) : QDialog(parent), ui(new Ui::AbsorptionDialog) {
    // Save reference to model
    this->model = model;
    this->domain = domain;
    
    // Setup the base UI
    ui->setupUi(this);
    
    // Set current absorption values in the spinboxes
    ui->spinBoxTop->setValue(model->domains[domain].getAbsorption(TOP));
    ui->spinBoxBottom->setValue(model->domains[domain].getAbsorption(BOTTOM));
    ui->spinBoxLeft->setValue(model->domains[domain].getAbsorption(LEFT));
    ui->spinBoxRight->setValue(model->domains[domain].getAbsorption(RIGHT));
    
    // Connect actions to the save and cancel buttons
    connect(ui->buttonCancel, SIGNAL(clicked(bool)), this, SLOT(slot_cancel()));
    connect(ui->buttonSave, SIGNAL(clicked(bool)), this, SLOT(slot_save()));
}

AbsorptionDialog::~AbsorptionDialog() {
    delete ui;
}

void AbsorptionDialog::slot_cancel() {
    // Close the window without saving
    close();
}

void AbsorptionDialog::slot_save() {
    // Save the new absorption values
    model->domains[domain].setAbsorption(TOP, ui->spinBoxTop->value());
    model->domains[domain].setAbsorption(BOTTOM, ui->spinBoxBottom->value());
    model->domains[domain].setAbsorption(LEFT, ui->spinBoxLeft->value());
    model->domains[domain].setAbsorption(RIGHT, ui->spinBoxRight->value());
    model->domains[domain].updateAbsorption();
    
    // Close the window
    close();
}