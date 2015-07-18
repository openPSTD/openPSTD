//
// Created by michiel on 18-7-2015.
//

#include "PSTDFile.h"
#include "external/unqlite.h"

std::unique_ptr<PSTDFile> PSTDFile::Open(const std::string &filename)
{
    auto result = new PSTDFile();
    result->changed = false;
    unqlite_open(&result->backend, filename.c_str(), UNQLITE_OPEN_CREATE);
    result->initilize();
    return result;
}

std::unique_ptr<PSTDFile> PSTDFile::New(const std::string &filename)
{
    //todo copy empty file
    return PSTDFile::Open(filename);
}

void PSTDFile::Close()
{
    unqlite_close(this->backend);
}

std::shared_ptr<rapidjson::Document> PSTDFile::GetSceneConf()
{
    return this->sceneConf;
}

void PSTDFile::SetSceneConf(std::shared_ptr<rapidjson::Document> scene)
{

}

std::shared_ptr<rapidjson::Document> PSTDFile::GetPSTDConf()
{
    return this->PSTDConf;
}

void PSTDFile::SetPSTDConf(std::shared_ptr<rapidjson::Document> scene)
{

}

int PSTDFile::GetFrameCount()
{
    return 0;
}

char *PSTDFile::GetFrame(int frame, int domain)
{
    return nullptr;
}

void PSTDFile::SaveNextFrame(int domain, const char *data)
{

}

void PSTDFile::DeleteSimulationResults()
{

}

void PSTDFile::Commit()
{

}

std::unique_ptr<std::string> PSTDFile::GetStringValue(const std::string &key)
{
    unqlite_int64 nBytes;  //Data length
    char *zBuf;     //Dynamically allocated buffer

    zBuf = GetRawValue(key, &nBytes);

    std::string* result = new std::string(zBuf);
    delete zBuf;

    return result;
}

char *PSTDFile::GetRawValue(const std::string &key, unqlite_int64* nBytes)
{
    int rc;
    char *zBuf;     //Dynamically allocated buffer

    rc = unqlite_kv_fetch(this->backend, key.c_str(), -1, NULL, nBytes);
    if( rc != UNQLITE_OK )
    {
        //todo throw error exception
    }

    zBuf = new char[*nBytes];

    unqlite_kv_fetch(this->backend, key.c_str(), -1, zBuf, nBytes);

    return zBuf;
}

void PSTDFile::initilize()
{
    std::unique_ptr<std::string> json;
    std::unique_ptr<rapidjson::Document> result;

    json = this->GetStringValue("scene");
    result = new rapidjson::Document();
    result->Parse(json->c_str());
    this->sceneConf = result;

    json = this->GetStringValue("PSTD");
    result = new rapidjson::Document();
    result->Parse(json->c_str());
    this->PSTDConf = result;

    changed = false;
}
