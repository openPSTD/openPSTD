#ifndef COORDINATEDIALOG_H
#define COORDINATEDIALOG_H

#include <QDialog>

namespace Ui {
    class CoordinateDialog;
}

/**
 * A dialog for requesting (x, y) coordinates.
 */
class CoordinateDialog : public QDialog {
    Q_OBJECT
public:
    // Constructor, destructor
    explicit CoordinateDialog(QWidget* parent = 0, QPoint initial = QPoint());
    ~CoordinateDialog();
    
    // Public output variables
    int x;
    int y;
private:
    Ui::CoordinateDialog* ui;
private slots:
    void slot_cancel();
    void slot_save();
};

#endif