#ifndef ABSORPTIONDIALOG_H
#define ABSORPTIONDIALOG_H

#include <QDialog>

namespace Ui {
    class AbsorptionDialog;
}

/**
 * A dialog for changing the absorption coefficients
 * of a domain's walls.
 */
class AbsorptionDialog : public QDialog {
    Q_OBJECT
public:
    // Constructor, destructor
    explicit AbsorptionDialog(QWidget* parent = 0, double top = 0, double bottom = 0, double left = 0, double right = 0);
    ~AbsorptionDialog();
    
    // Public output variables
    double top;
    double bottom;
    double left;
    double right;
private:
    Ui::AbsorptionDialog* ui;
private slots:
    void slot_cancel();
    void slot_save();
};

#endif