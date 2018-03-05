/********************************************************************************
** Form generated from reading UI file 'absorptiondialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABSORPTIONDIALOG_H
#define UI_ABSORPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_AbsorptionDialog
{
public:
    QFrame *frame;
    QDoubleSpinBox *spinBoxLeft;
    QDoubleSpinBox *spinBoxTop;
    QDoubleSpinBox *spinBoxBottom;
    QDoubleSpinBox *spinBoxRight;
    QPushButton *buttonCancel;
    QPushButton *buttonSave;

    void setupUi(QDialog *AbsorptionDialog)
    {
        if (AbsorptionDialog->objectName().isEmpty())
            AbsorptionDialog->setObjectName(QStringLiteral("AbsorptionDialog"));
        AbsorptionDialog->resize(320, 240);
        frame = new QFrame(AbsorptionDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(90, 40, 141, 121));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        spinBoxLeft = new QDoubleSpinBox(AbsorptionDialog);
        spinBoxLeft->setObjectName(QStringLiteral("spinBoxLeft"));
        spinBoxLeft->setGeometry(QRect(10, 90, 66, 24));
        spinBoxLeft->setMaximum(1);
        spinBoxLeft->setSingleStep(0.05);
        spinBoxTop = new QDoubleSpinBox(AbsorptionDialog);
        spinBoxTop->setObjectName(QStringLiteral("spinBoxTop"));
        spinBoxTop->setGeometry(QRect(130, 10, 66, 24));
        spinBoxTop->setMaximum(1);
        spinBoxTop->setSingleStep(0.05);
        spinBoxBottom = new QDoubleSpinBox(AbsorptionDialog);
        spinBoxBottom->setObjectName(QStringLiteral("spinBoxBottom"));
        spinBoxBottom->setGeometry(QRect(130, 170, 66, 24));
        spinBoxBottom->setMaximum(1);
        spinBoxBottom->setSingleStep(0.05);
        spinBoxRight = new QDoubleSpinBox(AbsorptionDialog);
        spinBoxRight->setObjectName(QStringLiteral("spinBoxRight"));
        spinBoxRight->setGeometry(QRect(240, 90, 66, 24));
        spinBoxRight->setMaximum(1);
        spinBoxRight->setSingleStep(0.05);
        buttonCancel = new QPushButton(AbsorptionDialog);
        buttonCancel->setObjectName(QStringLiteral("buttonCancel"));
        buttonCancel->setGeometry(QRect(20, 200, 80, 23));
        buttonSave = new QPushButton(AbsorptionDialog);
        buttonSave->setObjectName(QStringLiteral("buttonSave"));
        buttonSave->setGeometry(QRect(220, 200, 80, 23));

        retranslateUi(AbsorptionDialog);

        QMetaObject::connectSlotsByName(AbsorptionDialog);
    } // setupUi

    void retranslateUi(QDialog *AbsorptionDialog)
    {
        AbsorptionDialog->setWindowTitle(QApplication::translate("AbsorptionDialog", "Absorption", Q_NULLPTR));
        buttonCancel->setText(QApplication::translate("AbsorptionDialog", "Cancel", Q_NULLPTR));
        buttonSave->setText(QApplication::translate("AbsorptionDialog", "Save", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AbsorptionDialog: public Ui_AbsorptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABSORPTIONDIALOG_H
