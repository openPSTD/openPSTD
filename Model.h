//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_MODEL_H
#define OPENPSTD_MODEL_H

#include <rapidjson/document.h>
#include <memory>
#include <vector>
#include <QtGui/qmatrix4x4.h>
#include "PSTDFile.h"

class Model
{
public:
    std::unique_ptr<PSTDFile> d;

    QMatrix4x4 view;
};


#endif //OPENPSTD_MODEL_H
