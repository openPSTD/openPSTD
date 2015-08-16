//
// Created by michiel on 16-8-2015.
//

#ifndef OPENPSTD_EDITOPERATIONS_H
#define OPENPSTD_EDITOPERATIONS_H

#include "BaseOperation.h"
#include <boost/numeric/ublas/vector.hpp>

class CreateDomainOperation: public BaseOperation
{
private:
    boost::numeric::ublas::vector<float> StartPoint;
    boost::numeric::ublas::vector<float> EndPoint;

public:
    CreateDomainOperation(boost::numeric::ublas::vector<float> StartPoint, boost::numeric::ublas::vector<float> EndPoint);

    virtual void Run(const Reciever &reciever) override;
};

class RemoveDomainOperation: public BaseOperation
{
private:
    int index;

public:
    RemoveDomainOperation(int index);

    virtual void Run(const Reciever &reciever) override;
};

class EditDomainPositionsOperation: public BaseOperation
{
private:
    int index;
    boost::numeric::ublas::vector<float> StartPoint;
    boost::numeric::ublas::vector<float> EndPoint;

public:
    EditDomainPositionsOperation(int index, boost::numeric::ublas::vector<float> startPoint, boost::numeric::ublas::vector<float> endPoint);

    virtual void Run(const Reciever &reciever) override;
};

class EditDomainEdgeAbsorptionOperation: public BaseOperation
{
private:
    int index;
    PSTD_DOMAIN_SIDE Side;
    float NewValue;

public:
    EditDomainEdgeAbsorptionOperation(int index, PSTD_DOMAIN_SIDE side, float newValue);

    virtual void Run(const Reciever &reciever) override;
};

template <typename T>
class EditPropertyOperation: public BaseOperation
{
private:
    const std::string Property;
    T NewValue;

public:
    EditPropertyOperation(const std::string property, T newValue): NewValue(newValue), Property(property)
    {
    }

    virtual void Run(const Reciever &reciever) override
    {
        using namespace rapidjson;

        std::shared_ptr<Document> conf = reciever.model->d->GetSceneConf();

        Document::AllocatorType& allocator = conf->GetAllocator();

        (*conf).AddMember(this->Property, this->NewValue, allocator);

        reciever.model->d->SetSceneConf(conf);
    }
};


#endif //OPENPSTD_EDITOPERATIONS_H

