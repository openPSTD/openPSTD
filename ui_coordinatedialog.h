/********************************************************************************
** Form generated from reading UI file 'coordinatedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COORDINATEDIALOG_H
#define UI_COORDINATEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_CoordinateDialog
{
public:
    QSpinBox *sbX;
    QSpinBox *sbY;
    QPushButton *buttonSave;
    QPushButton *buttonCancel;
    QLabel *labelX;
    QLabel *labelY;

    void setupUi(QDialog *CoordinateDialog)
    {
        if (CoordinateDialog->objectName().isEmpty())
            CoordinateDialog->setObjectName(QStringLiteral("CoordinateDialog"));
        CoordinateDialog->resize(240, 174);
        sbX = new QSpinBox(CoordinateDialog);
        sbX->setObjectName(QStringLiteral("sbX"));
        sbX->setGeometry(QRect(100, 20, 101, 24));
        sbX->setMinimum(-1000000);
        sbX->setMaximum(1000000);
        sbY = new QSpinBox(CoordinateDialog);
        sbY->setObjectName(QStringLiteral("sbY"));
        sbY->setGeometry(QRect(100, 50, 101, 24));
        sbY->setMinimum(-1000000);
        sbY->setMaximum(1000000);
        buttonSave = new QPushButton(CoordinateDialog);
        buttonSave->setObjectName(QStringLiteral("buttonSave"));
        buttonSave->setGeometry(QRect(150, 140, 80, 23));
        buttonCancel = new QPushButton(CoordinateDialog);
        buttonCancel->setObjectName(QStringLiteral("buttonCancel"));
        buttonCancel->setGeometry(QRect(10, 140, 80, 23));
        labelX = new QLabel(CoordinateDialog);
        labelX->setObjectName(QStringLiteral("labelX"));
        labelX->setGeometry(QRect(10, 20, 81, 16));
        labelY = new QLabel(CoordinateDialog);
        labelY->setObjectName(QStringLiteral("labelY"));
        labelY->setGeometry(QRect(10, 50, 81, 16));

        retranslateUi(CoordinateDialog);

        QMetaObject::connectSlotsByName(CoordinateDialog);
    } // setupUi

    void retranslateUi(QDialog *CoordinateDialog)
    {
        CoordinateDialog->setWindowTitle(QApplication::translate("CoordinateDialog", "Dialog", Q_NULLPTR));
        buttonSave->setText(QApplication::translate("CoordinateDialog", "Save", Q_NULLPTR));
        buttonCancel->setText(QApplication::translate("CoordinateDialog", "Cancel", Q_NULLPTR));
        labelX->setText(QApplication::translate("CoordinateDialog", "x-coordinate:", Q_NULLPTR));
        labelY->setText(QApplication::translate("CoordinateDialog", "y-coordinate:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CoordinateDialog: public Ui_CoordinateDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COORDINATEDIALOG_H
