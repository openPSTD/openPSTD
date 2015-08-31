//
// Created by michiel on 31-8-2015.
//

#include "PstdAlgorithm.h"

bool PointInSquare(QVector2D tl, QVector2D size, QVector2D point)
{
    QVector2D br = tl+size;
    if(((tl[0] < point[0] && point[0] < br[0]) || (br[0] < point[0] && point[0] < tl[0]))
                && ((tl[1] < point[1] && point[1] < br[1]) || (br[1] < point[1] && point[1] < tl[1])))
    {
        return true;
    }
    else
    {
        return false;
    }
}