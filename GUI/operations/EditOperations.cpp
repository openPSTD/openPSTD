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
// Date: 16-08-2015
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////


#include "EditOperations.h"
#include "../Snapping.h"
#include "SelectionOperations.h"

CreateDomainOperation::CreateDomainOperation(QVector2D startPoint,
                                             QVector2D endPoint): StartPoint(startPoint), EndPoint(endPoint)
{

}

void CreateDomainOperation::Run(const Reciever &reciever)
{
    auto conf = reciever.model->d->GetSceneConf();

    this->StartPoint = Snapping::Snap(reciever.model, this->StartPoint);
    this->EndPoint = Snapping::Snap(reciever.model, this->EndPoint);

    DomainConf d;
    d.TopLeft.setX(std::min(this->StartPoint[0], this->EndPoint[0]));
    d.TopLeft.setY(std::min(this->StartPoint[1], this->EndPoint[1]));

    d.Size.setX(fabsf(this->StartPoint[0] - this->EndPoint[0]));
    d.Size.setY(fabsf(this->StartPoint[1] - this->EndPoint[1]));

    d.T.Absorption = 1.0;
    d.T.LR = false;

    d.B.Absorption = 1.0;
    d.B.LR = false;

    d.R.Absorption = 1.0;
    d.R.LR = false;

    d.L.Absorption = 1.0;
    d.L.LR = false;

    conf->Domains.push_back(d);

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

CreateReceiverSpeakerOperation::CreateReceiverSpeakerOperation(PstdObjectType type, QVector3D position) : _type(type),
                                                                                                _position(position)
{

}

void CreateReceiverSpeakerOperation::Run(const Reciever &reciever)
{
    auto conf = reciever.model->d->GetSceneConf();
    if(this->_type == OBJECT_RECEIVER)
    {
        conf->Receivers.push_back(this->_position);
    }
    else if(this->_type == OBJECT_SPEAKER)
    {
        conf->Speakers.push_back(this->_position);
    }
    else
    {
        //todo throw exception here
    }

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

void RemoveSelectedDomainOperation::Run(const Reciever &reciever)
{
    if(reciever.model->interactive->Selection.Type == SELECTION_DOMAIN)
    {
        int index = reciever.model->interactive->Selection.SelectedIndex;
        std::shared_ptr<RemoveDomainOperation> op1(new RemoveDomainOperation(index));
        reciever.operationRunner->RunOperation(op1);
        std::shared_ptr<DeselectOperation> op2(new DeselectOperation());
        reciever.operationRunner->RunOperation(op2);
    }
    else
    {
        //todo throw exception
    }
}

void RemoveSelectedObjectOperation::Run(const Reciever &reciever)
{
    int index = reciever.model->interactive->Selection.SelectedIndex;
    if(reciever.model->interactive->Selection.Type == SELECTION_DOMAIN)
    {
        std::shared_ptr<RemoveDomainOperation> op1(new RemoveDomainOperation(index));
        reciever.operationRunner->RunOperation(op1);
    }
    else if(reciever.model->interactive->Selection.Type == SELECTION_RECEIVER)
    {
        std::shared_ptr<RemoveReceiverOperation> op1(new RemoveReceiverOperation(index));
        reciever.operationRunner->RunOperation(op1);
    }
    else if(reciever.model->interactive->Selection.Type == SELECTION_SPEAKER)
    {
        std::shared_ptr<RemoveSpeakerOperation> op1(new RemoveSpeakerOperation(index));
        reciever.operationRunner->RunOperation(op1);
    }
    else
    {
        //todo throw exception
    }
    std::shared_ptr<DeselectOperation> op2(new DeselectOperation());
    reciever.operationRunner->RunOperation(op2);
}

RemoveDomainOperation::RemoveDomainOperation(int index): index(index)
{

}

void RemoveDomainOperation::Run(const Reciever &reciever)
{
    auto conf = reciever.model->d->GetSceneConf();

    conf->Domains.erase(conf->Domains.begin()+this->index);

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

RemoveSpeakerOperation::RemoveSpeakerOperation(int index): index(index)
{

}

void RemoveSpeakerOperation::Run(const Reciever &reciever)
{
    auto conf = reciever.model->d->GetSceneConf();

    conf->Speakers.erase(conf->Speakers.begin()+this->index);

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

RemoveReceiverOperation::RemoveReceiverOperation(int index): index(index)
{

}

void RemoveReceiverOperation::Run(const Reciever &reciever)
{
    auto conf = reciever.model->d->GetSceneConf();

    conf->Receivers.erase(conf->Receivers.begin()+this->index);

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

EditDomainPositionsOperation::EditDomainPositionsOperation(int index, QVector2D startPoint, QVector2D endPoint):
        index(index), StartPoint(startPoint), EndPoint(endPoint)
{

}

void EditDomainPositionsOperation::Run(const Reciever &reciever)
{
    auto conf = reciever.model->d->GetSceneConf();

    this->StartPoint = Snapping::Snap(reciever.model, this->StartPoint);
    this->EndPoint = Snapping::Snap(reciever.model, this->EndPoint);

    conf->Domains[index].TopLeft = this->StartPoint;
    conf->Domains[index].Size = this->StartPoint - this->EndPoint;

    conf->Domains[index].Size[0] = fabsf(conf->Domains[index].Size[0]);
    conf->Domains[index].Size[1] = fabsf(conf->Domains[index].Size[1]);

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

EditDomainEdgeAbsorptionOperation::EditDomainEdgeAbsorptionOperation(int index, PSTD_DOMAIN_SIDE side, float newValue):
index(index), Side(side), NewValue(newValue)
{

}

void EditDomainEdgeAbsorptionOperation::Run(const Reciever &reciever)
{
    auto conf = reciever.model->d->GetSceneConf();

    switch(this->Side)
    {
        case PSTD_DOMAIN_SIDE_BOTTOM:
            conf->Domains[index].B.Absorption = this->NewValue;
            break;
        case PSTD_DOMAIN_SIDE_LEFT:
            conf->Domains[index].L.Absorption = this->NewValue;
            break;
        case PSTD_DOMAIN_SIDE_RIGHT:
            conf->Domains[index].R.Absorption = this->NewValue;
            break;
        case PSTD_DOMAIN_SIDE_TOP:
            conf->Domains[index].T.Absorption = this->NewValue;
            break;
    }

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

EditDomainEdgeLrOperation::EditDomainEdgeLrOperation(int index, PSTD_DOMAIN_SIDE side, bool newValue):
index(index), Side(side), NewValue(newValue)
{
}

void EditDomainEdgeLrOperation::Run(const Reciever &reciever)
{

    auto conf = reciever.model->d->GetSceneConf();

    switch(this->Side)
    {
        case PSTD_DOMAIN_SIDE_BOTTOM:
            conf->Domains[index].B.LR = this->NewValue;
            break;
        case PSTD_DOMAIN_SIDE_LEFT:
            conf->Domains[index].L.LR = this->NewValue;
            break;
        case PSTD_DOMAIN_SIDE_RIGHT:
            conf->Domains[index].R.LR = this->NewValue;
            break;
        case PSTD_DOMAIN_SIDE_TOP:
            conf->Domains[index].T.LR = this->NewValue;
            break;
    }

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

void EditSelectedDomainEdgesOperation::Run(const Reciever &reciever)
{
    auto conf = reciever.model->d->GetSceneConf();
    int index = reciever.model->interactive->Selection.SelectedIndex;

    conf->Domains[index].T.Absorption = this->AbsorptionT;
    conf->Domains[index].B.Absorption = this->AbsorptionB;
    conf->Domains[index].L.Absorption = this->AbsorptionL;
    conf->Domains[index].R.Absorption = this->AbsorptionR;

    conf->Domains[index].T.LR = this->LRT;
    conf->Domains[index].B.LR = this->LRB;
    conf->Domains[index].L.LR = this->LRL;
    conf->Domains[index].R.LR = this->LRR;

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

EditDocumentSettingsOperation::EditDocumentSettingsOperation(PSTDSettings settings): Settings(settings)
{

}

void EditDocumentSettingsOperation::Run(const Reciever &reciever)
{
    auto conf = reciever.model->d->GetSceneConf();
    conf->Settings = this->Settings;
    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}