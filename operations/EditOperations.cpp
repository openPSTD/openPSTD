//
// Created by michiel on 16-8-2015.
//

#include "EditOperations.h"

CreateDomainOperation::CreateDomainOperation(QVector2D startPoint,
                                             QVector2D endPoint): StartPoint(startPoint), EndPoint(endPoint)
{

}

void CreateDomainOperation::Run(const Reciever &reciever)
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    float gridSpacing = (*conf)["grid_spacing"].GetDouble();

    this->StartPoint[0] = roundf(this->StartPoint[0]/gridSpacing)*gridSpacing;
    this->StartPoint[1] = roundf(this->StartPoint[1]/gridSpacing)*gridSpacing;
    this->EndPoint[0] = roundf(this->EndPoint[0]/gridSpacing)*gridSpacing;
    this->EndPoint[1] = roundf(this->EndPoint[1]/gridSpacing)*gridSpacing;

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

EditDomainPositionsOperation::EditDomainPositionsOperation(int index, boost::numeric::ublas::vector<float> startPoint,
                                         boost::numeric::ublas::vector<float> endPoint): index(index),
                                        StartPoint(startPoint), EndPoint(endPoint)
{

}

void EditDomainPositionsOperation::Run(const Reciever &reciever)
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

    float gridSpacing = (*conf)["grid_spacing"].GetDouble();

    this->StartPoint[0] = roundf(this->StartPoint[0]/gridSpacing)*gridSpacing;
    this->StartPoint[1] = roundf(this->StartPoint[1]/gridSpacing)*gridSpacing;
    this->EndPoint[0] = roundf(this->EndPoint[0]/gridSpacing)*gridSpacing;
    this->EndPoint[1] = roundf(this->EndPoint[1]/gridSpacing)*gridSpacing;

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
