#include "absorptiondialog.h"
#include "ui_absorptiondialog.h"

AbsorptionDialog::AbsorptionDialog(
	QWidget* parent,
	Domain* domain
) : QDialog(parent), ui(new Ui::AbsorptionDialog) {
	// Save reference to model
	this->domain = domain;

	// Setup the base UI
	ui->setupUi(this);

	// Set current absorption values in the spinboxes
	ui->spinBoxTop->setValue(*(domain->getAbsorption(TOP)));
	ui->spinBoxBottom->setValue(*(domain->getAbsorption(BOTTOM)));
	ui->spinBoxLeft->setValue(*(domain->getAbsorption(LEFT)));
	ui->spinBoxRight->setValue(*(domain->getAbsorption(RIGHT)));

	// Set current edge local reacting values in the checkboxes
	ui->elrTop->setChecked(*(domain->getEdgeLocalReacting(TOP)));
	ui->elrBottom->setChecked(*(domain->getEdgeLocalReacting(BOTTOM)));
	ui->elrLeft->setChecked(*(domain->getEdgeLocalReacting(LEFT)));
	ui->elrRight->setChecked(*(domain->getEdgeLocalReacting(RIGHT)));

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
	domain->setAbsorption(TOP, ui->spinBoxTop->value());
	domain->setAbsorption(BOTTOM, ui->spinBoxBottom->value());
	domain->setAbsorption(LEFT, ui->spinBoxLeft->value());
	domain->setAbsorption(RIGHT, ui->spinBoxRight->value());
	
	// Save the edge local reacting values
	domain->setEdgeLocalReacting(TOP, ui->elrTop->checkState());
	domain->setEdgeLocalReacting(BOTTOM, ui->elrBottom->checkState());
	domain->setEdgeLocalReacting(LEFT, ui->elrLeft->checkState());
	domain->setEdgeLocalReacting(RIGHT, ui->elrRight->checkState());

	// Close the window
	close();
}