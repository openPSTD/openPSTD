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
}

void RemoveSelectedDomainOperation::Run(const Reciever &reciever)
{
    int index = reciever.model->interactive->SelectedDomainIndex;
    if(index >= 0)
    {
        std::shared_ptr<RemoveDomainOperation> op1(new RemoveDomainOperation(index));
        reciever.operationRunner->RunOperation(op1);
        std::shared_ptr<SelectDomainOperation> op2(new SelectDomainOperation(-1));
        reciever.operationRunner->RunOperation(op2);
    }
    else
    {
        //todo throw exception
    }
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

    Value topLeft(kArrayType);
    topLeft.PushBack(std::min(this->StartPoint[0], this->EndPoint[0]), allocator);
    topLeft.PushBack(std::min(this->StartPoint[1], this->EndPoint[1]), allocator);

    Value size(kArrayType);
    size.PushBack(fabsf(this->StartPoint[0] - this->EndPoint[0]), allocator);
    size.PushBack(fabsf(this->StartPoint[1] - this->EndPoint[1]), allocator);

    (*conf)["domains"][index].AddMember("topleft", topLeft, allocator);
    (*conf)["domains"][index].AddMember("size", size, allocator);

    reciever.model->d->SetSceneConf(conf);
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

    (*conf)["domains"][index]["edges"][DomainSideToString(this->Side)].AddMember("a", this->NewValue, allocator);

    reciever.model->d->SetSceneConf(conf);
}
