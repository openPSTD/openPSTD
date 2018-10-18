#include "coordinatedialog.h"
#include "ui_coordinatedialog.h"

CoordinateDialog::CoordinateDialog(
	QWidget* parent,
	bool isSource
) : QDialog(parent), ui(new Ui::CoordinateDialog) {
	// Save reference to model (TODO)
	this->isSource = isSource;

	// Setup the base UI
	ui->setupUi(this);

	// Connect actions to the save and cancel buttons
	connect(ui->buttonCancel, SIGNAL(clicked(bool)), this, SLOT(slot_cancel()));
	connect(ui->buttonSave, SIGNAL(clicked(bool)), this, SLOT(slot_save()));

	// Set the title of the dialog
	if (isSource) {
		setWindowTitle("Add source");
	} else {
		setWindowTitle("Add receiver");
	}

	// Focus the x-coordinate spinbox
	ui->sbX->setFocus();
}

CoordinateDialog::~CoordinateDialog() {
	delete ui;
}

QPoint CoordinateDialog::getCoords() {
	return coords;
}

void CoordinateDialog::slot_cancel() {
	// Close the window without saving
	saved = false;
	close();
}

void CoordinateDialog::slot_save() {
	// Close the window
	coords = QPoint(
		ui->sbX->value(),
		ui->sbY->value()
	);
	saved = true;
	close();
}