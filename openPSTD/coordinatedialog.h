#ifndef COORDINATEDIALOG_H
#define COORDINATEDIALOG_H

#include <QDialog>
#include "model.h"
#include "settings.h"

namespace Ui {
    class CoordinateDialog;
}

class CoordinateDialog : public QDialog {
    Q_OBJECT
public:
    explicit CoordinateDialog(QWidget* parent = 0, Model* model = 0, bool isSource = true);
    ~CoordinateDialog();
private:
    Ui::CoordinateDialog* ui;
    Model* model;
    bool isSource;
private slots:
    void slot_cancel();
    void slot_save();
};

#endif