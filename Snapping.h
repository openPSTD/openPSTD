//
// Created by michiel on 30-8-2015.
//

#ifndef OPENPSTD_SNAPPING_H
#define OPENPSTD_SNAPPING_H

#include <QVector2D>
#include "Model.h"

class Snapping
{
public:
    static QVector2D Snap(std::shared_ptr<Model> const &model, QVector2D vector);
};


#endif //OPENPSTD_SNAPPING_H
