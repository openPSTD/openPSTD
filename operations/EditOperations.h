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
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_EDITOPERATIONS_H
#define OPENPSTD_EDITOPERATIONS_H

#include "BaseOperation.h"
#include <boost/numeric/ublas/vector.hpp>
#include <QVector2D>

class CreateDomainOperation: public BaseOperation
{
private:
    QVector2D StartPoint;
    QVector2D EndPoint;

public:
    CreateDomainOperation(QVector2D StartPoint, QVector2D EndPoint);

    virtual void Run(const Reciever &reciever) override;
};

class CreateReceiverSpeakerOperation: public BaseOperation
{
private:
    PstdObjectType _type;
    QVector3D _position;

public:
    CreateReceiverSpeakerOperation(PstdObjectType type, QVector3D position);

    virtual void Run(const Reciever &reciever) override;
};

class RemoveSelectedDomainOperation: public BaseOperation
{
public:
    virtual void Run(const Reciever &reciever) override;
};

class RemoveSelectedObjectOperation: public BaseOperation
{
public:
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

class RemoveSpeakerOperation: public BaseOperation
{
private:
    int index;

public:
    RemoveSpeakerOperation(int index);

    virtual void Run(const Reciever &reciever) override;
};

class RemoveReceiverOperation: public BaseOperation
{
private:
    int index;

public:
    RemoveReceiverOperation(int index);

    virtual void Run(const Reciever &reciever) override;
};

class EditDomainPositionsOperation: public BaseOperation
{
private:
    int index;
    QVector2D StartPoint;
    QVector2D EndPoint;

public:
    EditDomainPositionsOperation(int index, QVector2D startPoint, QVector2D endPoint);

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

class EditDomainEdgeLrOperation: public BaseOperation
{
private:
    int index;
    PSTD_DOMAIN_SIDE Side;
    bool NewValue;

public:
    EditDomainEdgeLrOperation(int index, PSTD_DOMAIN_SIDE side, bool newValue);

    virtual void Run(const Reciever &reciever) override;
};

class EditSelectedDomainEdgesOperation: public BaseOperation
{
public:
    float AbsorptionT;
    float AbsorptionB;
    float AbsorptionL;
    float AbsorptionR;
    bool LRT;
    bool LRB;
    bool LRL;
    bool LRR;

    virtual void Run(const Reciever &reciever) override;
};

class EditDocumentSettingsOperation: public BaseOperation
{
public:
    EditDocumentSettingsOperation(){};
    EditDocumentSettingsOperation(PSTDFileSettings settings);
    PSTDFileSettings Settings;
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

