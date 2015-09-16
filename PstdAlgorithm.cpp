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

void AddSquareBuffer(std::unique_ptr<std::vector<float>> const &buffer, QVector2D tl, QVector2D size)
{
    float left = tl[0];
    float top = tl[1];

    float right = tl[0]+size[0];
    float bottom = tl[1]+size[1];

    buffer->push_back(left); buffer->push_back(top);
    buffer->push_back(right); buffer->push_back(top);

    buffer->push_back(left); buffer->push_back(top);
    buffer->push_back(left); buffer->push_back(bottom);

    buffer->push_back(left); buffer->push_back(bottom);
    buffer->push_back(right); buffer->push_back(bottom);

    buffer->push_back(right); buffer->push_back(top);
    buffer->push_back(right); buffer->push_back(bottom);
}

void AddSquareBuffer(std::unique_ptr<std::vector<QVector2D>> const &buffer, QVector2D tl, QVector2D size)
{
    float left = tl[0];
    float top = tl[1];

    float right = tl[0]+size[0];
    float bottom = tl[1]+size[1];

    buffer->push_back(QVector2D(left, top));
    buffer->push_back(QVector2D(right, top));

    buffer->push_back(QVector2D(left, top));
    buffer->push_back(QVector2D(left, bottom));

    buffer->push_back(QVector2D(left, bottom));
    buffer->push_back(QVector2D(right, bottom));

    buffer->push_back(QVector2D(right, top));
    buffer->push_back(QVector2D(right, bottom));
}