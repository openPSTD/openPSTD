//
// Created by michiel on 1-9-2015.
//

#ifndef OPENPSTD_DOMAINPROPERTIES_H
#define OPENPSTD_DOMAINPROPERTIES_H

#include <QDialog>
#include <ui_DomainProperties.h>
#include <memory>

class DomainProperties: public QDialog
{
    Q_OBJECT

public:
    explicit DomainProperties(QWidget *parent = 0);
    float AbsorptionT();
    float AbsorptionB();
    float AbsorptionL();
    float AbsorptionR();
    bool LRT();
    bool LRB();
    bool LRL();
    bool LRR();

    void SetAbsorptionT(float value);
    void SetAbsorptionB(float value);
    void SetAbsorptionL(float value);
    void SetAbsorptionR(float value);
    void SetLRT(bool value);
    void SetLRB(bool value);
    void SetLRL(bool value);
    void SetLRR(bool value);

private:
    std::unique_ptr<Ui_DomainProperties> ui;
};


#endif //OPENPSTD_DOMAINPROPERTIES_H
