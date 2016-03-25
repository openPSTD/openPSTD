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

namespace OpenPSTD
{
    namespace GUI
    {

        OpenFileOperation::OpenFileOperation(std::string filename) : filename(filename)
        {
        }

        void OpenFileOperation::Run(const Reciever &reciever)
        {
            reciever.model->documentAccess->Open(this->filename);
            reciever.model->documentAccess->Change();
        }


        NewFileOperation::NewFileOperation(std::string filename) : filename(filename)
        {
        }

        void NewFileOperation::Run(const Reciever &reciever)
        {
            reciever.model->documentAccess->New(this->filename);
            reciever.model->documentAccess->Change();
        }

        void SaveFileOperation::Run(const Reciever &reciever)
        {
            reciever.model->documentAccess->Save();
        }

        SaveAsFileOperation::SaveAsFileOperation(std::string filename): filename(filename)
        {
        }

        void SaveAsFileOperation::Run(const Reciever &reciever)
        {
            reciever.model->documentAccess->SaveAs(this->filename);
        }
    }
}
