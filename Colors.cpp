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

#include "Colors.h"

std::unique_ptr<std::vector<float>> BaseColorGradient::CreateRawRGBAColorMap(float start, float stop, unsigned int count)
{
    std::unique_ptr<std::vector<QColor>> map = this->CreateColorMap(start, stop, count);
    std::unique_ptr<std::vector<float>> result(new std::vector<float>(count*4));
    for(int i = 0; i < count; i++)
    {
        (*result)[i*4+0] = (*map)[i].redF();
        (*result)[i*4+1] = (*map)[i].greenF();
        (*result)[i*4+2] = (*map)[i].blueF();
        (*result)[i*4+3] = (*map)[i].alphaF();
    }
    return std::move(result);
}

std::unique_ptr<std::vector<QColor>> BaseColorGradient::CreateColorMap(float start, float stop, unsigned int count)
{
    std::unique_ptr<std::vector<QColor>> result(new std::vector<QColor>(count));
    float diff = stop-start;
    float step = diff/(count-1); //the -1 is that the stop is including in the colormap
    for(int i = 0; i < count; i++)
    {
        (*result)[i] = this->CalcColor(start+i*step);
    }
    return std::move(result);
}

void MultiColorGradient::AddColor(float position, QColor c)
{
    this->colors[position] = c;
}

QColor MultiColorGradient::CalcColor(float value)
{
    std::map<float, QColor>::iterator upperBound = this->colors.upper_bound(value);
    if(upperBound == this->colors.begin())
    {
        return upperBound->second;
    }
    std::map<float, QColor>::iterator lowerBound = upperBound--;
    if(upperBound == this->colors.end())
    {
        return lowerBound->second;
    }

    QColor result;
    result.setRedF(this->LinearInterpolation(lowerBound->first, lowerBound->second.redF(), upperBound->first, upperBound->second.redF(), value));
    result.setGreenF(this->LinearInterpolation(lowerBound->first, lowerBound->second.greenF(), upperBound->first, upperBound->second.greenF(), value));
    result.setBlueF(this->LinearInterpolation(lowerBound->first, lowerBound->second.blueF(), upperBound->first, upperBound->second.blueF(), value));
    result.setAlphaF(this->LinearInterpolation(lowerBound->first, lowerBound->second.alphaF(), upperBound->first, upperBound->second.alphaF(), value));

    return result;
}

float MultiColorGradient::LinearInterpolation(float x0, float y0, float x1, float y1, float x)
{
    return y0 + (y1 - y0)*((x-x0)/(x1-x0));
}

TwoColorGradient::TwoColorGradient(QColor c0, QColor c1)
{
    this->AddColor(0, c0);
    this->AddColor(1, c1);
}

QColor BaseColorScheme::EditorBackgroundColor() { return COLOR_WHITE; }
QColor BaseColorScheme::EditorLineColor() { return COLOR_WHITE; }
std::unique_ptr<BaseColorGradient> BaseColorScheme::EditorLineAbsoptionColorGradient() { return COLOR_GRADIENT_WHITE; }
QColor BaseColorScheme::EditorDefualtDomainColor() { return COLOR_WHITE; }
std::unique_ptr<BaseColorGradient> BaseColorScheme::EditorDomainSignalColorGradient() { return COLOR_GRADIENT_WHITE; }
QColor BaseColorScheme::EditorAddDomainColor(){ return COLOR_WHITE; }



QColor StandardColorScheme::EditorBackgroundColor()
{
    return COLOR_WHITE;
}

QColor StandardColorScheme::EditorLineColor()
{
    return COLOR_BLACK;
}

std::unique_ptr<BaseColorGradient> StandardColorScheme::EditorLineAbsoptionColorGradient()
{
    return std::unique_ptr<TwoColorGradient>(new TwoColorGradient(COLOR_BLUE, COLOR_GREEN));
}

QColor StandardColorScheme::EditorDefualtDomainColor()
{
    return COLOR_RED;
}

std::unique_ptr<BaseColorGradient> StandardColorScheme::EditorDomainSignalColorGradient()
{
    std::unique_ptr<MultiColorGradient> result(new MultiColorGradient());
    result->AddColor(0, COLOR_BLACK);
    result->AddColor(0.10f, COLOR_RED);
    return std::move(result);
}

QColor StandardColorScheme::EditorAddDomainColor()
{
    return COLOR_YELLOW;
}
