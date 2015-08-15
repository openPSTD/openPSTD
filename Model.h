//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_MODEL_H
#define OPENPSTD_MODEL_H

#include <rapidjson/document.h>
#include <memory>
#include <vector>
#include "PSTDFile.h"

class Model
{
public:
    std::unique_ptr<PSTDFile> d;
};


#endif //OPENPSTD_MODEL_H
