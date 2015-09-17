//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date:
//
//
// Authors:
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

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
