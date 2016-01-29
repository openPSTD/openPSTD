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
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 1-9-2015.
//

#include "DomainProperties.h"

namespace OpenPSTD
{
    namespace GUI
    {
        DomainProperties::DomainProperties(QWidget *parent) : QDialog(parent), ui(new Ui_DomainProperties())
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

        void DomainProperties::SetDomain(DomainConf d)
        {
            this->SetAbsorptionT(d.T.Absorption);
            this->SetAbsorptionB(d.B.Absorption);
            this->SetAbsorptionL(d.L.Absorption);
            this->SetAbsorptionR(d.R.Absorption);

            this->SetLRT(d.T.LR);
            this->SetLRB(d.B.LR);
            this->SetLRL(d.L.LR);
            this->SetLRR(d.R.LR);

            this->_d = d;
        }

        DomainConf DomainProperties::GetDomain()
        {
            DomainConf result = this->_d;

            result.L.LR = this->LRL();
            result.R.LR = this->LRR();
            result.B.LR = this->LRB();
            result.T.LR = this->LRT();

            result.L.Absorption = this->AbsorptionL();
            result.R.Absorption = this->AbsorptionR();
            result.B.Absorption = this->AbsorptionB();
            result.T.Absorption = this->AbsorptionT();

            return result;
        }
    }
}