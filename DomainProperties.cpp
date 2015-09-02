//
// Created by michiel on 1-9-2015.
//

#include "DomainProperties.h"

DomainProperties::DomainProperties(QWidget *parent):QDialog(parent), ui(new Ui_DomainProperties())
{
    ui->setupUi(this);
}
float DomainProperties::AbsorptionT()
{
    return ui->absorptionRateT->value();
}
float DomainProperties::AbsorptionB()
{
    return ui->absorptionRateB->value();
}
float DomainProperties::AbsorptionL()
{
    return ui->absorptionRateL->value();
}
float DomainProperties::AbsorptionR()
{
    return ui->absorptionRateR->value();
}
bool DomainProperties::LRT()
{
    return ui->LRT->isChecked();
}
bool DomainProperties::LRB()
{
    return ui->LRB->isChecked();
}
bool DomainProperties::LRL()
{
    return ui->LRL->isChecked();
}
bool DomainProperties::LRR()
{
    return ui->LRR->isChecked();
}

void DomainProperties::SetAbsorptionT(float value)
{
    ui->absorptionRateT->setValue(value);
}
void DomainProperties::SetAbsorptionB(float value)
{
    ui->absorptionRateB->setValue(value);
}
void DomainProperties::SetAbsorptionL(float value)
{
    ui->absorptionRateL->setValue(value);
}
void DomainProperties::SetAbsorptionR(float value)
{
    ui->absorptionRateR->setValue(value);
}
void DomainProperties::SetLRT(bool value)
{
    ui->LRT->setChecked(value);
}
void DomainProperties::SetLRB(bool value)
{
    ui->LRB->setChecked(value);
}
void DomainProperties::SetLRL(bool value)
{
    ui->LRL->setChecked(value);
}
void DomainProperties::SetLRR(bool value)
{
    ui->LRR->setChecked(value);
}