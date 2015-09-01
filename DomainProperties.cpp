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
    return 0.0f;
}
float DomainProperties::AbsorptionB()
{
    return 0.0f;
}
float DomainProperties::AbsorptionL()
{
    return 0.0f;
}
float DomainProperties::AbsorptionR()
{
    return 0.0f;
}
bool DomainProperties::LRT()
{
    return false;
}
bool DomainProperties::LRB()
{
    return false;
}
bool DomainProperties::LRL()
{
    return false;
}
bool DomainProperties::LRR()
{
    return false;
}