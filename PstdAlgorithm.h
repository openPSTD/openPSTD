//
// Created by michiel on 31-8-2015.
//

#ifndef OPENPSTD_ALGORITHM_H
#define OPENPSTD_ALGORITHM_H
#include <QVector2D>
#include <vector>
#include <memory>

bool PointInSquare(QVector2D tl, QVector2D size, QVector2D point);

void AddSquareBuffer(std::unique_ptr<std::vector<float>> const &buffer, QVector2D tl, QVector2D size);
void AddSquareBuffer(std::unique_ptr<std::vector<QVector2D>> const &buffer, QVector2D tl, QVector2D size);
QVector4D ToVectorRGBA(QColor color);

#endif //OPENPSTD_ALGORITHM_H
