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
// Created by michiel on 16-8-2015.
//

#include "ViewOperations.h"
#include <algorithm>

namespace OpenPSTD
{
    namespace GUI
    {
        void UpdateViewMatrix::Run(const Reciever &reciever)
        {
            reciever.model->view->viewMatrix = reciever.model->view->aspectMatrix * reciever.model->view->worldMatrix;
            reciever.model->view->Change();
        }

        ChangeAspectMatrix::ChangeAspectMatrix(QMatrix4x4 m) : M(m)
        {

        }

        ChangeAspectMatrix::ChangeAspectMatrix(float w, float h) : M()
        {
            this->M.scale(h / w, 1);
        }

        void ChangeAspectMatrix::Run(const Reciever &reciever)
        {
            reciever.model->view->aspectMatrix = this->M;
            reciever.model->view->Change();
            reciever.operationRunner->RunOperation(std::shared_ptr<UpdateViewMatrix>(new UpdateViewMatrix()));
        }

        ChangeWorldMatrix::ChangeWorldMatrix(QMatrix4x4 m) : M(m)
        {

        }

        void ChangeWorldMatrix::Run(const Reciever &reciever)
        {
            reciever.model->view->worldMatrix = this->M;
            reciever.model->view->Change();
            reciever.operationRunner->RunOperation(std::shared_ptr<UpdateViewMatrix>(new UpdateViewMatrix()));
        }

        ModifyWorldMatrix::ModifyWorldMatrix(QMatrix4x4 m) : M(m)
        {

        }

        void ModifyWorldMatrix::Run(const Reciever &reciever)
        {
            reciever.model->view->worldMatrix = this->M * reciever.model->view->worldMatrix;
            reciever.model->view->Change();
            reciever.operationRunner->RunOperation(std::shared_ptr<UpdateViewMatrix>(new UpdateViewMatrix()));
        }

        TranslateScene::TranslateScene(QVector2D vector) : ModifyWorldMatrix()
        {
            this->M.translate(vector.toVector3D());
        }

        ResizeScene::ResizeScene(float scale, QVector2D pos) : ModifyWorldMatrix()
        {
            this->M.translate(pos.toVector3D());
            this->M.scale(scale);
            this->M.translate(-pos.toVector3D());
        }

        void ViewWholeScene::Run(const Reciever &reciever)
        {
            //todo fix this command
        }

        ChangeViewingFrame::ChangeViewingFrame(unsigned int newFrame): frame(newFrame)
        {

        }

        void ChangeViewingFrame::Run(const Reciever &reciever)
        {
            auto doc = reciever.model->documentAccess->GetDocument();
            int min = INT_MAX;
            int max = INT_MIN;
            int domains = doc->GetResultsDomainCount();

            for(int d = 0; d < domains; d++)
            {
                int frameCount = doc->GetResultsFrameCount(d);
                if(frameCount > 0)
                {
                    min = 0;
                }
                max = std::max(max, frameCount);
            }

            if(min <= this->frame && this->frame <= max)
            {
                std::cout << "show frame " << this->frame << std::endl;
                reciever.model->interactive->visibleFrame = this->frame;
                reciever.model->interactive->Change();
            }
        }
    }
}





