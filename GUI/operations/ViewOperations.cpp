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
            // the minus 1 to make it conform the file format
            this->M.scale(h / w, -1);
        }

        void ChangeAspectMatrix::Run(const Reciever &reciever)
        {
            reciever.model->view->aspectMatrix = this->M;
            reciever.model->view->Change();
            reciever.operationRunner->RunOperation(std::make_shared<UpdateViewMatrix>());
        }

        ChangeWorldMatrix::ChangeWorldMatrix(QMatrix4x4 m) : M(m)
        {

        }

        void ChangeWorldMatrix::Run(const Reciever &reciever)
        {
            reciever.model->view->worldMatrix = this->M;
            reciever.model->view->Change();
            reciever.operationRunner->RunOperation(std::make_shared<UpdateViewMatrix>());
        }

        ModifyWorldMatrix::ModifyWorldMatrix(QMatrix4x4 m) : M(m)
        {

        }

        void ModifyWorldMatrix::Run(const Reciever &reciever)
        {
            reciever.model->view->worldMatrix = this->M * reciever.model->view->worldMatrix;
            reciever.model->view->Change();
            reciever.operationRunner->RunOperation(std::make_shared<UpdateViewMatrix>());
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
            QVector2D tl, br;

            if(reciever.model->documentAccess->IsDocumentLoaded())
            {
                auto doc = reciever.model->documentAccess->GetDocument();
                auto conf = doc->GetSceneConf();

                if (conf->Domains.size() > 0)
                {
                    //initilize the tl and br with the first element
                    tl[0] = conf->Domains[0].TopLeft[0];
                    tl[1] = conf->Domains[0].TopLeft[1];
                    br[0] = conf->Domains[0].TopLeft[0] + conf->Domains[0].Size[0];
                    br[1] = conf->Domains[0].TopLeft[1] + conf->Domains[0].Size[1];

                    //check if there are more extreme
                    for (int i = 1; i < conf->Domains.size(); i++)
                    {
                        tl[0] = std::min(tl[0], conf->Domains[i].TopLeft[0]);
                        tl[1] = std::min(tl[1], conf->Domains[i].TopLeft[1]);
                        br[0] = std::max(br[0], conf->Domains[i].TopLeft[0] + conf->Domains[i].Size[0]);
                        br[1] = std::max(br[1], conf->Domains[i].TopLeft[1] + conf->Domains[i].Size[1]);
                    }
                    for (int i = 1; i < conf->Receivers.size(); i++)
                    {
                        tl[0] = std::min(tl[0], conf->Receivers[i][0]);
                        tl[1] = std::min(tl[1], conf->Receivers[i][1]);
                        br[0] = std::max(br[0], conf->Receivers[i][0]);
                        br[1] = std::max(br[1], conf->Receivers[i][1]);
                    }
                    for (int i = 1; i < conf->Speakers.size(); i++)
                    {
                        tl[0] = std::min(tl[0], conf->Speakers[i][0]);
                        tl[1] = std::min(tl[1], conf->Speakers[i][1]);
                        br[0] = std::max(br[0], conf->Speakers[i][0]);
                        br[1] = std::max(br[1], conf->Speakers[i][1]);
                    }

                    //calculate the center
                    QVector2D center((tl[0] + br[0]) / 2, (tl[1] + br[1]) / 2);
                    //calculate how much is should zoom out
                    float scaleFactor = 2 / (ExtraZoomFactor * std::max(fabs(br[0] - tl[0]), fabs(br[1] - tl[1])));

                    //zoom out
                    this->M.scale(scaleFactor);

                    //translate to the center
                    this->M.translate(-center);

                    //exectute the matrix
                    ChangeWorldMatrix::Run(reciever);
                }
            }
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
                reciever.model->interactive->visibleFrame = this->frame;
                reciever.model->interactive->Change();
            }
        }
    }
}





