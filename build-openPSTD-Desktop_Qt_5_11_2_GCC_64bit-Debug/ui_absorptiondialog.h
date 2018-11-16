/********************************************************************************
** Form generated from reading UI file 'absorptiondialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABSORPTIONDIALOG_H
#define UI_ABSORPTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
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
    QCheckBox *elrTop;
    QCheckBox *elrBottom;
    QCheckBox *elrLeft;
    QCheckBox *elrRight;

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
        elrTop = new QCheckBox(AbsorptionDialog);
        elrTop->setObjectName(QStringLiteral("elrTop"));
        elrTop->setGeometry(QRect(200, 10, 51, 21));
        elrBottom = new QCheckBox(AbsorptionDialog);
        elrBottom->setObjectName(QStringLiteral("elrBottom"));
        elrBottom->setGeometry(QRect(200, 170, 51, 21));
        elrLeft = new QCheckBox(AbsorptionDialog);
        elrLeft->setObjectName(QStringLiteral("elrLeft"));
        elrLeft->setGeometry(QRect(10, 120, 51, 21));
        elrRight = new QCheckBox(AbsorptionDialog);
        elrRight->setObjectName(QStringLiteral("elrRight"));
        elrRight->setGeometry(QRect(240, 120, 51, 21));

        retranslateUi(AbsorptionDialog);

        QMetaObject::connectSlotsByName(AbsorptionDialog);
    } // setupUi

    void retranslateUi(QDialog *AbsorptionDialog)
    {
        AbsorptionDialog->setWindowTitle(QApplication::translate("AbsorptionDialog", "Absorption", nullptr));
        buttonCancel->setText(QApplication::translate("AbsorptionDialog", "Cancel", nullptr));
        buttonSave->setText(QApplication::translate("AbsorptionDialog", "Save", nullptr));
        elrTop->setText(QApplication::translate("AbsorptionDialog", "ELR", nullptr));
        elrBottom->setText(QApplication::translate("AbsorptionDialog", "ELR", nullptr));
        elrLeft->setText(QApplication::translate("AbsorptionDialog", "ELR", nullptr));
        elrRight->setText(QApplication::translate("AbsorptionDialog", "ELR", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AbsorptionDialog: public Ui_AbsorptionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABSORPTIONDIALOG_H
