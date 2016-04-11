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
// Date: 18-07-2015
//
//
// Authors: M. R. Fortuin
//
//
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////


#ifndef OPENPSTD_MODEL_H
#define OPENPSTD_MODEL_H

#include <memory>
#include <vector>
#include <QtGui/qmatrix4x4.h>
#include <kernel/KernelInterface.h>
#include <shared/PSTDFile.h>
#include <shared/Colors.h>
#include <shared/PSTDFileAccess.h>
#include "Settings.h"

namespace OpenPSTD
{
    namespace GUI
    {
        class MouseStrategy;

        enum SelectionType
        {
            SELECTION_NONE,
            SELECTION_DOMAIN,
            SELECTION_RECEIVER,
            SELECTION_SPEAKER
        };

        enum PstdObjectType
        {
            OBJECT_NONE,
            OBJECT_DOMAIN,
            OBJECT_RECEIVER,
            OBJECT_SPEAKER
        };

        class InteractiveModel : public Shared::InvalidationData
        {
        public:
            struct
            {
            public:
                bool visible;
                QVector2D start;
                QVector2D currentEnd;
            } CreateDomain;

            struct
            {
                int SelectedIndex;
                SelectionType Type;
            } Selection;
        };

        class View : public Shared::InvalidationData
        {
        public:
            QMatrix4x4 viewMatrix;
            QMatrix4x4 worldMatrix;
            QMatrix4x4 aspectMatrix;
        };

        class Model : public Shared::InvalidationData
        {
        public:
            Model();

            std::shared_ptr<OpenPSTD::Shared::PSTDFileAccess> documentAccess;
            std::shared_ptr<View> view;
            std::unique_ptr<MouseStrategy> mouseHandler;
            std::shared_ptr<InteractiveModel> interactive;
            std::shared_ptr<Settings> settings;
            std::unique_ptr<Shared::BaseColorScheme> colorScheme;
        };

    }
}

#endif //OPENPSTD_MODEL_H
