#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QFormLayout>
#include "modelmanager.h"
#include <iostream>

class Sidebar : public QWidget {
	Q_OBJECT
public:
	Sidebar(QWidget* parent = nullptr);
	~Sidebar();
	
	// TODO
	inline QSize minimumSizeHint() const { return QSize(300, 300); }
	
	void updateVisibility();
private:
	// Form layout
	QFormLayout* layout;
	
	// Setting: Grid spacing
	QLabel* label_gridspacing;
	QDoubleSpinBox* value_gridspacing;
	
	// Setting: Window size
	QLabel* label_windowsize;
	QSpinBox* value_windowsize;
	
	// Setting: Render time
	QLabel* label_rendertime;
	QDoubleSpinBox* value_rendertime;
	
	// Setting: Air density
	QLabel* label_airdensity;
	QDoubleSpinBox* value_airdensity;
	
	// Setting: Sound speed
	QLabel* label_soundspeed;
	QSpinBox* value_soundspeed;
	
	// Setting: PML cells
	QLabel* label_pmlcells;
	QSpinBox* value_pmlcells;
	
	// Setting: Attenuation of PML cells
	QLabel* label_attenuationpmlcells;
	QSpinBox* value_attenuationpmlcells;
	
	void loadValues();
	void saveValues();
};

#endif