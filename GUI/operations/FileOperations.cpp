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
// Created by michiel on 18-7-2015.
//

#include "FileOperations.h"
#include "long/LongOperationRunner.h"
#include <shared/export/HDF5Export.h>
#include <shared/export/Image.h>

namespace OpenPSTD
{
    namespace GUI
    {

        OpenFileOperation::OpenFileOperation(std::string filename) : filename(filename)
        {
        }

        void OpenFileOperation::Run(const Reciever &reciever)
        {
            if(!reciever.Backgroundworker->IsIdle())
                std::cout << "Error: background worker is still running" << std::endl;
                //todo give correct error
            else
                reciever.model->documentAccess->Open(this->filename);
        }


        NewFileOperation::NewFileOperation(std::string filename) : filename(filename)
        {
        }

        void NewFileOperation::Run(const Reciever &reciever)
        {
            if(!reciever.Backgroundworker->IsIdle())
                std::cout << "Error: background worker is still running" << std::endl;
                //todo give correct error
            else
                reciever.model->documentAccess->New(this->filename);
        }

        void SaveFileOperation::Run(const Reciever &reciever)
        {
            if(!reciever.Backgroundworker->IsIdle())
                std::cout << "Error: background worker is still running" << std::endl;
                //todo give correct error
            else
                reciever.model->documentAccess->Save();
        }

        SaveAsFileOperation::SaveAsFileOperation(std::string filename): filename(filename)
        {
        }

        void SaveAsFileOperation::Run(const Reciever &reciever)
        {
            if(!reciever.Backgroundworker->IsIdle())
                std::cout << "Error: background worker is still running" << std::endl;
                //todo give correct error
            else
                reciever.model->documentAccess->SaveAs(this->filename);
        }

        ExportToHDF5Operation::ExportToHDF5Operation(std::string filename): filename(filename)
        {

        }

        void ExportToHDF5Operation::Run(const Reciever &reciever)
        {
            std::vector<int> domains;
            auto doc = reciever.model->documentAccess->GetDocument();
            OpenPSTD::Shared::HDF5 realExport;
            //doc-> is strange, but the API needs an shared_ptr, but the lock is kept because the locking_ptr is destroyed only
            //when outside the scope of the method
            realExport.ExportData("application/x-hdf", doc.get(), this->filename, domains, -1, -1);
        }


        ExportToImageOperation::ExportToImageOperation(std::string directory, std::string name,
                                                          OpenPSTD::GUI::ImageFormat format): directory(directory),
                                                                                              name(name),
                                                                                              format(format),
                                                                                              startFrame(0),
                                                                                              endFrame(-1)
        {

        }

        void ExportToImageOperation::Run(const Reciever &reciever)
        {
            auto doc = reciever.model->documentAccess->GetDocument();
            std::vector<int> domains;

            OpenPSTD::Shared::ExportImage realExport;
            realExport.SetFullView(true);
            std::string formatString;
            if(this->format == ImageFormat::BMP)
            {
                formatString = "image/bmp";
            }
            else if(this->format == ImageFormat::PNG)
            {
                formatString = "image/png";
            }
            else
            {
                formatString = "image/jpg";
            }
            realExport.ExportData(formatString, doc.get(), this->directory, this->name, domains, this->startFrame, this->endFrame);
        }

    }
}




