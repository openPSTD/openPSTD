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
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 30-8-2015.
//

#ifndef OPENPSTD_COLORS_H
#define OPENPSTD_COLORS_H

#include "openpstd-shared_export.h"
#include <qcolor.h>
#include <vector>
#include <memory>
#include <map>

namespace OpenPSTD
{
    namespace Shared
    {

/**
 * A base class that calculates color gradients
 */
        class OPENPSTD_SHARED_EXPORT BaseColorGradient
        {
        public:
            /**
             * Calculates color from a value in a certain gradient
             * @param value the value that is as basis of the gradient calculation. This value should be between 0 and 1 (including)
             * @return The color
             */
            virtual OPENPSTD_SHARED_EXPORT QColor CalcColor(float value) = 0;

            /**
             * Creates a complete color map,
             * @param start The minimal value of the colormap
             * @param stop The maximal value of the colormap
             * @param count The number of samples taken
             * @return the complete colormap
             */
            OPENPSTD_SHARED_EXPORT virtual std::unique_ptr<std::vector<QColor>> CreateColorMap(float start, float stop, unsigned int count);

            /**
             * Creates a complete color map of RGB values,
             * @param start The minimal value of the colormap
             * @param stop The maximal value of the colormap
             * @param count The number of samples taken
             * @return the complete colormap
             */
            OPENPSTD_SHARED_EXPORT virtual std::unique_ptr<std::vector<QRgb>> CreateColorRGBMap(float start, float stop, unsigned int count);

            /**
             * Creates a raw color map with RGBA values.
             * @param start The minimal value of the colormap
             * @param stop The maximal value of the colormap
             * @param count The number of samples taken
             * @return the complete raw colormap
             */
            OPENPSTD_SHARED_EXPORT std::unique_ptr<std::vector<float>> CreateRawRGBAColorMap(float start, float stop, unsigned int count);
        };

/**
 * A multicolor gradient where there are multiple colors at certain locations.
 * Outside the 2 outer colors, the color is constant based on the closest of color.
 */
        class OPENPSTD_SHARED_EXPORT MultiColorGradient : public BaseColorGradient
        {
        public:
            /**
             * Adds a color the the gradient at a certain position
             * @param position The position of the color, this should be between 0 and 1 (including)
             * @param c The color at that position
             */
            OPENPSTD_SHARED_EXPORT void AddColor(float position, QColor c);

            OPENPSTD_SHARED_EXPORT virtual QColor CalcColor(float value);

        private:
            std::map<float, QColor> colors;

            OPENPSTD_SHARED_NO_EXPORT float LinearInterpolation(float x0, float y0, float x1, float y1, float x);
        };

/**
 * A 2-color gradient where 1 color is used at 0 and the other color is used at 1.
 * This class is a specilized case of the MultiColorGradient
 */
        class OPENPSTD_SHARED_EXPORT TwoColorGradient : public MultiColorGradient
        {
        public:
            OPENPSTD_SHARED_EXPORT TwoColorGradient(QColor c0, QColor c1);
        };

#define COLOR_BLACK QColor(0, 0, 0, 255)
#define COLOR_WHITE QColor(255, 255, 255, 255)
#define COLOR_RED QColor(255, 0, 0, 255)
#define COLOR_GREEN QColor(0, 255, 0, 255)
#define COLOR_BLUE QColor(0, 0, 255, 255)
#define COLOR_YELLOW QColor(255, 255, 0, 255)
#define COLOR_LIGHT_BLUE QColor(0, 255, 255, 255)
#define COLOR_PURPLE QColor(255, 0, 255, 255)
#define COLOR_TRANSPARENT QColor(0, 0, 0, 0)
#define COLOR_GRADIENT_WHITE std::unique_ptr<TwoColorGradient>(new TwoColorGradient(COLOR_WHITE, COLOR_WHITE))

/**
 * A base color scheme, just defines the property's
 */
        class OPENPSTD_SHARED_EXPORT BaseColorScheme
        {
        public:
            OPENPSTD_SHARED_EXPORT virtual QColor EditorBackgroundColor();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorLineColor();

            OPENPSTD_SHARED_EXPORT virtual std::unique_ptr<BaseColorGradient> EditorLineAbsoptionColorGradient();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorDefualtDomainColor();

            OPENPSTD_SHARED_EXPORT virtual std::unique_ptr<BaseColorGradient> EditorDomainSignalColorGradient();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorAddDomainColor();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorSelectionColor();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorSpeakerColor();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorReceiverColor();

        };

/**
 * The default color scheme
 */
        class OPENPSTD_SHARED_EXPORT StandardColorScheme : public BaseColorScheme
        {
        public:
            OPENPSTD_SHARED_EXPORT virtual QColor EditorBackgroundColor();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorLineColor();

            OPENPSTD_SHARED_EXPORT virtual std::unique_ptr<BaseColorGradient> EditorLineAbsoptionColorGradient();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorDefualtDomainColor();

            OPENPSTD_SHARED_EXPORT virtual std::unique_ptr<BaseColorGradient> EditorDomainSignalColorGradient();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorAddDomainColor();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorSelectionColor();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorSpeakerColor();

            OPENPSTD_SHARED_EXPORT virtual QColor EditorReceiverColor();

        };
    }
}

#endif //OPENPSTD_COLORS_H
