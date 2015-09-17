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
// Date:
//
//
// Authors:
//
//
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 30-8-2015.
//

#include "Snapping.h"
#include <algorithm>

QVector2D Snapping::Snap(std::shared_ptr<Model> const &model, QVector2D vector)
{
    bool snap0 = false;
    bool snap1 = false;
    if(model->settings->snapping.SnapToDomainEdges)
    {
        std::unique_ptr<std::vector<float>> edges0 = GetEdges(model, 0);
        std::unique_ptr<std::vector<float>> edges1 = GetEdges(model, 1);
        if(edges0->size() > 0 && edges1->size() > 0)
        {
            float min0 = *std::min_element(edges0->begin(), edges0->end(), [&vector](const float& x, const float& y){return abs(x-vector[0]) < abs(y-vector[0]);});
            float min1 = *std::min_element(edges1->begin(), edges1->end(), [&vector](const float& x, const float& y){return abs(x-vector[1]) < abs(y-vector[1]);});
            if(abs(min0 - vector[0]) < model->settings->snapping.SnapToDomainEdgesDistance)
            {
                vector[0] = min0;
                snap0 = true;
            }
            if(abs(min1 - vector[1]) < model->settings->snapping.SnapToDomainEdgesDistance)
            {
                vector[1] = min1;
                snap1 = true;
            }
        }
    }
    if(model->settings->snapping.SnapToGrid)
    {
        std::shared_ptr<rapidjson::Document> conf = model->d->GetSceneConf();
        float gridSpacing = (*conf)["grid_spacing"].GetDouble();

        if(!snap0)
            vector[0] = roundf(vector[0] / gridSpacing) * gridSpacing;
        if(!snap1)
            vector[1] = roundf(vector[1] / gridSpacing) * gridSpacing;
    }

    return vector;
}

std::unique_ptr<std::vector<float>> Snapping::GetEdges(std::shared_ptr<Model> const &model, int dimension)
{
    std::unique_ptr<std::vector<float>> result(new std::vector<float>());
    std::shared_ptr<rapidjson::Document> conf = model->d->GetSceneConf();

    rapidjson::Value& domains = (*conf)["domains"];

    for(rapidjson::SizeType i = 0; i < domains.Size(); i++)
    {
        float first = domains[i]["topleft"][dimension].GetDouble();
        float second = first+domains[i]["size"][dimension].GetDouble();

        result->push_back(first);
        result->push_back(second);
    }
    return std::move(result);
}