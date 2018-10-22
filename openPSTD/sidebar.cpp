#include "sidebar.h"

Sidebar::Sidebar(QWidget* parent) : QWidget(parent) {
	// Hide the sidebar by default
	setVisible(false);
	
	// Setup a form layout for the settings items
	layout = new QFormLayout();
	setLayout(layout);
	
	// Setting: Grid spacing
	label_gridspacing = new QLabel("Grid spacing", this);
	value_gridspacing = new QDoubleSpinBox(this);
	layout->addRow(label_gridspacing, value_gridspacing);
	
	// Setting: Window size
	label_windowsize = new QLabel("Window size", this);
	value_windowsize = new QSpinBox(this);
	value_windowsize->setMaximum(1E9);
	layout->addRow(label_windowsize, value_windowsize);
	
	// Setting: Render time
	label_rendertime = new QLabel("Render time", this);
	value_rendertime = new QDoubleSpinBox(this);
	layout->addRow(label_rendertime, value_rendertime);
	
	// Setting: Air density
	label_airdensity = new QLabel("Air density", this);
	value_airdensity = new QDoubleSpinBox(this);
	layout->addRow(label_airdensity, value_airdensity);
	
	// Setting: Sound speed
	label_soundspeed = new QLabel("Sound speed", this);
	value_soundspeed = new QSpinBox(this);
	value_soundspeed->setMaximum(1E9);
	layout->addRow(label_soundspeed, value_soundspeed);
	
	// Setting: PML cells
	label_pmlcells = new QLabel("PML cells", this);
	value_pmlcells = new QSpinBox(this);
	value_pmlcells->setMaximum(1E9);
	layout->addRow(label_pmlcells, value_pmlcells);
	
	// Setting: Attenuation of PML cells
	label_attenuationpmlcells = new QLabel("Attenuation of PML cells", this);
	value_attenuationpmlcells = new QSpinBox(this);
	value_attenuationpmlcells->setMaximum(1E9);
	layout->addRow(label_attenuationpmlcells, value_attenuationpmlcells);
}

Sidebar::~Sidebar() {
	// Delete all rows in the form
	delete label_gridspacing;
	delete value_gridspacing;
	delete label_windowsize;
	delete value_windowsize;
	delete label_rendertime;
	delete value_rendertime;
	delete label_airdensity;
	delete value_airdensity;
	delete label_soundspeed;
	delete value_soundspeed;
	delete label_pmlcells;
	delete value_pmlcells;
	delete label_attenuationpmlcells;
	delete value_attenuationpmlcells;
	
	// Delete the form layout
	delete layout;
}

void Sidebar::updateVisibility() {
	// Get the state of the settings action
	Model* model = ModelManager::getInstance()->getCurrent();
	bool visible = model->actionShowSidebar->isChecked();
	
	// Update the values
	if (isVisible()) {
		saveValues();
	} else {
		loadValues();
	}
	
	// Update the visibility of the sidebar accordingly
	setVisible(visible);
}

void Sidebar::loadValues() {
	Model* model = ModelManager::getInstance()->getCurrent();
	value_gridspacing->setValue(model->gridspacing);
	value_windowsize->setValue(model->windowsize);
	value_rendertime->setValue(model->rendertime);
	value_airdensity->setValue(model->airdensity);
	value_soundspeed->setValue(model->soundspeed);
	value_pmlcells->setValue(model->pmlcells);
	value_attenuationpmlcells->setValue(model->attenuationpmlcells);
}

void Sidebar::saveValues() {
	Model* model = ModelManager::getInstance()->getCurrent();
	model->gridspacing = value_gridspacing->value();
	model->windowsize = value_windowsize->value();
	model->rendertime = value_rendertime->value();
	model->airdensity = value_airdensity->value();
	model->soundspeed = value_soundspeed->value();
	model->pmlcells = value_pmlcells->value();
	model->attenuationpmlcells = value_attenuationpmlcells->value();
}