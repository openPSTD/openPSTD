#include "coordinatedialog.h"
#include "ui_coordinatedialog.h"

CoordinateDialog::CoordinateDialog(QWidget *parent, Model* model, Settings* settings, bool isSource) : QDialog(parent), ui(new Ui::CoordinateDialog) {
    // Save reference to model
    this->model = model;
    this->settings = settings;
    this->isSource = isSource;
    
    // Setup the base UI
    ui->setupUi(this);
    
    // Connect actions to the save and cancel buttons
    connect(ui->buttonCancel, SIGNAL(clicked(bool)), this, SLOT(slot_cancel()));
    connect(ui->buttonSave, SIGNAL(clicked(bool)), this, SLOT(slot_save()));
    
    // Focus the x-coordinate spinbox
    ui->sbX->setFocus();
}

CoordinateDialog::~CoordinateDialog() {
    delete ui;
}

void CoordinateDialog::slot_cancel() {
    // Close the window without saving
    close();
}

void CoordinateDialog::slot_save() {
    if (isSource) {
        // Add a new source to the model
        model->sources.push_back(Source(
            ui->sbX->value(),
            -ui->sbY->value(),
            settings
        ));
    } else {
        // Add a new receiver to the model
        model->receivers.push_back(Receiver(
            ui->sbX->value(),
            -ui->sbY->value(),
            settings
        ));
    }
    
    // Close the window
    close();
}