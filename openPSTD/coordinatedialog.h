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
    explicit CoordinateDialog(QWidget* parent = 0, Model* model = 0, Settings* settings = 0, bool isSource = true);
    ~CoordinateDialog();
private:
    Ui::CoordinateDialog* ui;
    Model* model;
    Settings* settings;
    bool isSource;
private slots:
    void slot_cancel();
    void slot_save();
};

#endif