#include "absorptiondialog.h"
#include "ui_absorptiondialog.h"

AbsorptionDialog::AbsorptionDialog(QWidget* parent, unsigned int domain, Model* model) : QDialog(parent), ui(new Ui::AbsorptionDialog) {
    // Save reference to model
    this->model = model;
    this->domain = domain;
    
    // Setup the base UI
    ui->setupUi(this);
    
    // Set current absorption values in the spinboxes
    std::vector<Wall*>* walls = model->domains[domain].getWalls();
    for (unsigned int i = 0; i < walls->size(); i++) {
        Wall* w = walls->at(i);
        if (w->getSide() == TOP)    ui->spinBoxTop->setValue(w->getAbsorption());
        if (w->getSide() == BOTTOM) ui->spinBoxBottom->setValue(w->getAbsorption());
        if (w->getSide() == LEFT)   ui->spinBoxLeft->setValue(w->getAbsorption());
        if (w->getSide() == RIGHT)  ui->spinBoxRight->setValue(w->getAbsorption());
    }
    
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
    std::vector<Wall*>* walls = model->domains[domain].getWalls();
    for (unsigned int i = 0; i < walls->size(); i++) {
        Wall* w = walls->at(i);
        if (w->getSide() == TOP)    w->setAbsorption(ui->spinBoxTop->value());
        if (w->getSide() == BOTTOM) w->setAbsorption(ui->spinBoxBottom->value());
        if (w->getSide() == LEFT)   w->setAbsorption(ui->spinBoxLeft->value());
        if (w->getSide() == RIGHT)  w->setAbsorption(ui->spinBoxRight->value());
    }
    
    // Close the window
    close();
}