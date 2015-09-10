//
// Created by michiel on 18-7-2015.
//

#include "FileOperations.h"
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/encodedstream.h>


OpenFileOperation::OpenFileOperation(std::string filename): filename(filename)
{
}

void OpenFileOperation::Run(const Reciever &reciever)
{
    reciever.model->d = PSTDFile::Open(this->filename);
    reciever.model->d->Change();
    reciever.model->Register(reciever.model->d);
}


NewFileOperation::NewFileOperation(std::string filename): filename(filename)
{
}

void NewFileOperation::Run(const Reciever &reciever)
{
    reciever.model->d = PSTDFile::New(this->filename);
    reciever.model->d->Change();
    reciever.model->Register(reciever.model->d);
}

void SaveFileOperation::Run(const Reciever &reciever)
{
    reciever.model->d->Commit();
    reciever.model->d->Change();
    reciever.model->Register(reciever.model->d);
}
