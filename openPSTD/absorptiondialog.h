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
    explicit AbsorptionDialog(QWidget* parent = 0, unsigned int domain = 0, Model* model = 0);
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