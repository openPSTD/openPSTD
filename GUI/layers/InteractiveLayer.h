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
//      30-8-2015
//
// Authors:
//      michiel
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_INTERACTIVELAYER_H
#define OPENPSTD_INTERACTIVELAYER_H

#include "GUI/Viewer2D.h"
namespace OpenPSTD
{
    namespace GUI
    {
        class InteractiveLayer : public Layer
        {
        private:
            std::unique_ptr<QOpenGLShaderProgram> program;
            bool addDomainVisible;
            bool selectionVisible;
            unsigned int newDomainBuffer;
            unsigned int selectionBuffer;
            QColor newDomainColor;
            QColor selectionColor;
        public:
            InteractiveLayer();

            virtual void InitializeGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f);

            virtual void PaintGL(QObject *context, std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f);

            virtual void UpdateScene(std::shared_ptr<Model> const &m,
                                     std::unique_ptr<QOpenGLFunctions, void (*)(void *)> const &f);

            virtual MinMaxValue GetMinMax();
        };

    }
}

#endif //OPENPSTD_INTERACTIVELAYER_H
