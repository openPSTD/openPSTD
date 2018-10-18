#ifndef COORDINATEDIALOG_H
#define COORDINATEDIALOG_H

#include <QDialog>
#include <QPoint>
#include "model.h"
#include "settings.h"

namespace Ui {
	class CoordinateDialog;
}

class CoordinateDialog : public QDialog {
	Q_OBJECT
public:
	explicit CoordinateDialog(
		QWidget* parent = nullptr,
		bool isSource = true
	);
	~CoordinateDialog();
	inline bool getSaved() { return saved; }
	QPoint getCoords();
private:
	Ui::CoordinateDialog* ui;
	bool isSource;
	bool saved;
	QPoint coords;
private slots:
	void slot_cancel();
	void slot_save();
};

#endif