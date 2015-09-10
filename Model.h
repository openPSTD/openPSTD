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
#include "InvalidationData.h"

class InteractiveModel: public InvalidationData
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

class Settings: public InvalidationData
{
public:
    SnappingSettings snapping;
    VisualSettings visual;
};

class View: public InvalidationData
{
public:
    QMatrix4x4 viewMatrix;
    QMatrix4x4 worldMatrix;
    QMatrix4x4 aspectMatrix;
};

class Model: public InvalidationData
{
public:
    Model();

    std::shared_ptr<PSTDFile> d;
    std::shared_ptr<View> view;
    std::unique_ptr<MouseStrategy> mouseHandler;
    std::shared_ptr<InteractiveModel> interactive;
    std::shared_ptr<Settings> settings;
};


#endif //OPENPSTD_MODEL_H
