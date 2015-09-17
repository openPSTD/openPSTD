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
// Created by michiel on 16-8-2015.
//

#include "EditOperations.h"
#include "Snapping.h"
#include "SelectDomainOperation.h"

CreateDomainOperation::CreateDomainOperation(QVector2D startPoint,
                                             QVector2D endPoint): StartPoint(startPoint), EndPoint(endPoint)
{

}

void CreateDomainOperation::Run(const Reciever &reciever)
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    this->StartPoint = Snapping::Snap(reciever.model, this->StartPoint);
    this->EndPoint = Snapping::Snap(reciever.model, this->EndPoint);

    Document::AllocatorType& allocator = conf->GetAllocator();

    Value topLeft(kArrayType);
    topLeft.PushBack(std::min(this->StartPoint[0], this->EndPoint[0]), allocator);
    topLeft.PushBack(std::min(this->StartPoint[1], this->EndPoint[1]), allocator);

    Value size(kArrayType);
    size.PushBack(fabsf(this->StartPoint[0] - this->EndPoint[0]), allocator);
    size.PushBack(fabsf(this->StartPoint[1] - this->EndPoint[1]), allocator);

    Value edgeL(kObjectType);
    edgeL.AddMember("a", 1.0, allocator);
    edgeL.AddMember("lr", false, allocator);

    Value edgeB(kObjectType);
    edgeB.AddMember("a", 1.0, allocator);
    edgeB.AddMember("lr", false, allocator);

    Value edgeT(kObjectType);
    edgeT.AddMember("a", 1.0, allocator);
    edgeT.AddMember("lr", false, allocator);

    Value edgeR(kObjectType);
    edgeR.AddMember("a", 1.0, allocator);
    edgeR.AddMember("lr", false, allocator);

    Value edges(kObjectType);
    edges.AddMember("l", edgeL, allocator);
    edges.AddMember("b", edgeB, allocator);
    edges.AddMember("t", edgeT, allocator);
    edges.AddMember("r", edgeR, allocator);

    Value Domain(kObjectType);
    Domain.AddMember("topleft", topLeft, allocator);
    Domain.AddMember("size", size, allocator);
    Domain.AddMember("edges", edges, allocator);

    (*conf)["domains"].PushBack(Domain, allocator);

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
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    (*conf)["domains"].Erase((*conf)["domains"].Begin()+this->index);;

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

RemoveSpeakerOperation::RemoveSpeakerOperation(int index): index(index)
{

}

void RemoveSpeakerOperation::Run(const Reciever &reciever)
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    (*conf)["speakers"].Erase((*conf)["speakers"].Begin()+this->index);;

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

RemoveReceiverOperation::RemoveReceiverOperation(int index): index(index)
{

}

void RemoveReceiverOperation::Run(const Reciever &reciever)
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    (*conf)["receivers"].Erase((*conf)["receivers"].Begin()+this->index);;

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

EditDomainPositionsOperation::EditDomainPositionsOperation(int index, QVector2D startPoint, QVector2D endPoint):
        index(index), StartPoint(startPoint), EndPoint(endPoint)
{

}

void EditDomainPositionsOperation::Run(const Reciever &reciever)
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    this->StartPoint = Snapping::Snap(reciever.model, this->StartPoint);
    this->EndPoint = Snapping::Snap(reciever.model, this->EndPoint);

    Document::AllocatorType& allocator = conf->GetAllocator();

    (*conf)["domains"][index]["topleft"][0] = std::min(this->StartPoint[0], this->EndPoint[0]);
    (*conf)["domains"][index]["topleft"][1] = std::min(this->StartPoint[1], this->EndPoint[1]);
    (*conf)["domains"][index]["size"][0] = fabsf(this->StartPoint[0] - this->EndPoint[0]);
    (*conf)["domains"][index]["size"][1] = fabsf(this->StartPoint[1] - this->EndPoint[1]);

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

EditDomainEdgeAbsorptionOperation::EditDomainEdgeAbsorptionOperation(int index, PSTD_DOMAIN_SIDE side, float newValue):
index(index), Side(side), NewValue(newValue)
{

}

void EditDomainEdgeAbsorptionOperation::Run(const Reciever &reciever)
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    Document::AllocatorType& allocator = conf->GetAllocator();

    (*conf)["domains"][index]["edges"][DomainSideToString(this->Side)]["a"] = this->NewValue;

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

EditDomainEdgeLrOperation::EditDomainEdgeLrOperation(int index, PSTD_DOMAIN_SIDE side, bool newValue):
index(index), Side(side), NewValue(newValue)
{
}

void EditDomainEdgeLrOperation::Run(const Reciever &reciever)
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    Document::AllocatorType& allocator = conf->GetAllocator();

    (*conf)["domains"][index]["edges"][DomainSideToString(this->Side)]["ls"] = this->NewValue;

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

void EditSelectedDomainEdgesOperation::Run(const Reciever &reciever)
{
    int index = reciever.model->interactive->Selection.SelectedIndex;
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    Document::AllocatorType& allocator = conf->GetAllocator();

    (*conf)["domains"][index]["edges"]["t"]["a"] = this->AbsorptionT;
    (*conf)["domains"][index]["edges"]["b"]["a"] = this->AbsorptionB;
    (*conf)["domains"][index]["edges"]["l"]["a"] = this->AbsorptionL;
    (*conf)["domains"][index]["edges"]["r"]["a"] = this->AbsorptionR;

    (*conf)["domains"][index]["edges"]["t"]["lr"] = this->LRT;
    (*conf)["domains"][index]["edges"]["b"]["lr"] = this->LRB;
    (*conf)["domains"][index]["edges"]["l"]["lr"] = this->LRL;
    (*conf)["domains"][index]["edges"]["r"]["lr"] = this->LRR;

    std::cout << this->AbsorptionT << " == " << (*conf)["domains"][index]["edges"]["t"]["a"].GetDouble() << std::endl;

    reciever.model->d->SetSceneConf(conf);
    reciever.model->d->Change();
}

EditDocumentSettingsOperation::EditDocumentSettingsOperation(PSTDFileSettings settings): Settings(settings)
{

}

void EditDocumentSettingsOperation::Run(const Reciever &reciever)
{
    reciever.model->d->SetSettings(this->Settings);
    reciever.model->d->Change();
}