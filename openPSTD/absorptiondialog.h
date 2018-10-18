#ifndef ABSORPTIONDIALOG_H
#define ABSORPTIONDIALOG_H

#include <QDialog>
#include "model.h"
#include "domain.h"

namespace Ui {
	class AbsorptionDialog;
}

class AbsorptionDialog : public QDialog {
	Q_OBJECT
public:
	explicit AbsorptionDialog(
		QWidget* parent = nullptr,
		Domain* domain = nullptr
	);
	~AbsorptionDialog();
private:
	Ui::AbsorptionDialog* ui;
	Domain* domain;
private slots:
	void slot_cancel();
	void slot_save();
};

#endif
