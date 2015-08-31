//
// Created by michiel on 30-8-2015.
//

#include "Snapping.h"

QVector2D Snapping::Snap(std::shared_ptr<Model> const &model, QVector2D vector)
{
    if(model->snapping->SnapToGrid)
    {
        std::shared_ptr<rapidjson::Document> conf = model->d->GetSceneConf();
        float gridSpacing = (*conf)["grid_spacing"].GetDouble();

        vector[0] = roundf(vector[0] / gridSpacing) * gridSpacing;
        vector[1] = roundf(vector[1] / gridSpacing) * gridSpacing;
    }

    return vector;
}