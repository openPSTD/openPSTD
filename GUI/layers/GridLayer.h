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
//      26-8-2015
//
// Authors:
//      michiel
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_GRIDLAYER_H
#define OPENPSTD_GRIDLAYER_H

#include "GUI/Viewer2D.h"

namespace OpenPSTD
{
    namespace GUI
    {
        class GridLayer : public Layer
        {
        private:
            std::unique_ptr<QOpenGLShaderProgram> program;

            void UpdateLines();
            void UpdateOriginLines();

            std::unique_ptr<std::vector<float>> positions;
            std::unique_ptr<std::vector<float>> originPositions;
            unsigned int positionsBuffer;
            unsigned int originPositionsBuffer;
            int lines;
            int originLines;
            float gridSpacing;
            QMatrix4x4 viewMatrix;
            QMatrix4x4 worldMatrix;

        public:
            GridLayer();

            virtual void InitializeGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f);

            virtual void PaintGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f);

            virtual void UpdateScene(std::shared_ptr<Model> const &m,
                                     std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f);

            virtual MinMaxValue GetMinMax();

            float CalcIdealSpacing();
        };

    }
}
#endif //OPENPSTD_GRIDLAYER_H
