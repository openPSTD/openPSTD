//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_MODEL_H
#define OPENPSTD_MODEL_H

class Model;

#include <rapidjson/document.h>
#include <memory>
#include <vector>
#include <QtGui/qmatrix4x4.h>
#include "PSTDFile.h"
#include "MouseHandlers.h"
#include <QVector2D>
#include "Colors.h"

class InvalidationDataItemI
{
public:
    InvalidationDataItemI() : changed(true)
    {

    }

    bool changed;
    bool IsChanged();
    void Change();
    void Reset();
};

template<typename T>
class InvalidationDataItem: public InvalidationDataItemI
{
public:
    T value;

    InvalidationDataItem()
    {

    }

    InvalidationDataItem(T internalValue): InvalidationDataItemI()
    {
        this->value = internalValue;
    }
    T Get()
    {
        return value;
    }

    void Set(T value)
    {
        this->value = value;
    }

    operator T&() { return value; }
    operator T() const { return value; }
};

class InvalidationData
{
private:
    std::vector<std::weak_ptr<InvalidationDataItemI>> items;

public:
    void Register(std::weak_ptr<InvalidationDataItemI> item);

    bool IsChanged();
    void Reset();
};

class InteractiveModel: public InvalidationDataItemI
{
public:
    struct {
    public:
        bool visible;
        QVector2D start;
        QVector2D currentEnd;
    } CreateDomain;
    int SelectedDomainIndex;
};

class SnappingSettings
{
public:
    bool SnapToGrid = true;
    bool SnapToDomainEdges = true;
    float SnapToDomainEdgesDistance = 0.4f;
};

class VisualSettings
{
public:
    float EdgeSize = 5.0f;
    std::unique_ptr<BaseColorScheme> colorScheme;
};

class Settings: public InvalidationDataItemI
{
public:
    SnappingSettings snapping;
    VisualSettings visual;
};

class View: public InvalidationDataItemI
{
public:
    QMatrix4x4 viewMatrix;
    QMatrix4x4 worldMatrix;
    QMatrix4x4 aspectMatrix;
};

class Model
{
public:
    Model();

    InvalidationData invalidation;
    std::unique_ptr<PSTDFile> d;
    std::shared_ptr<View> view;
    std::unique_ptr<MouseStrategy> mouseHandler;
    std::shared_ptr<InteractiveModel> interactive;
    std::shared_ptr<Settings> settings;
};


#endif //OPENPSTD_MODEL_H
