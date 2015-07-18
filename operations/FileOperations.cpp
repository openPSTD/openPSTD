//
// Created by michiel on 18-7-2015.
//

#include "FileOperations.h"
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/encodedstream.h>


OpenFileOperation::OpenFileOperation(std::string filename)
{
    this->filename = filename;
}

void OpenFileOperation::Run(const Reciever &reciever)
{
    using namespace rapidjson;

    FILE* fp = fopen("big.json", "rb"); // non-Windows use "r"
    char readBuffer[65536];
    FileReadStream bis(fp, readBuffer, sizeof(readBuffer));

    AutoUTFInputStream<unsigned, FileReadStream> eis(bis);  // wraps bis into eis

    reciever.model->d = new Document();
    reciever.model->d->ParseStream<0, AutoUTF<unsigned>>(eis);

    fclose(fp);
}


