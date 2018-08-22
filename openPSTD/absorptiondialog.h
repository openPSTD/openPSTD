#ifndef ABSORPTIONDIALOG_H
#define ABSORPTIONDIALOG_H

#include <QDialog>
#include "model.h"

namespace Ui {
	class AbsorptionDialog;
}

class AbsorptionDialog : public QDialog {
	Q_OBJECT
public:
	explicit AbsorptionDialog(
		QWidget* parent = nullptr,
		unsigned int domain = 0,
		Model* model = nullptr
	);
	~AbsorptionDialog();
private:
	Ui::AbsorptionDialog* ui;
	Model* model;
	unsigned int domain;
private slots:
	void slot_cancel();
	void slot_save();
};

#endif
