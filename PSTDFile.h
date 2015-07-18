//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_PSTDFILE_H
#define OPENPSTD_PSTDFILE_H

#include <string>
#include <external/unqlite.h>
#include "rapidjson/document.h"
#include <memory>

class PSTDFile
{
private:
    bool changed;
    std::unique_ptr<rapidjson::Document> sceneConf;
    std::unique_ptr<rapidjson::Document> PSTDConf;
    unqlite* backend;

    std::unique_ptr<std::string> GetStringValue(const std::string &key);
    char* GetRawValue(const std::string &key, unqlite_int64* nBytes);

    void initilize();
public:
    static std::unique_ptr<PSTDFile> Open(const std::string &filename);
    static std::unique_ptr<PSTDFile> New(const std::string &filename);
    void Commit();
    void Close();

    std::shared_ptr<rapidjson::Document> GetSceneConf();
    void SetSceneConf(std::shared_ptr<rapidjson::Document> scene);

    std::shared_ptr<rapidjson::Document> GetPSTDConf();
    void SetPSTDConf(std::shared_ptr<rapidjson::Document> scene);

    int GetFrameCount();

    char* GetFrame(int frame, int domain);
    void SaveNextFrame(int domain, const char* data);

    void DeleteSimulationResults();
};


#endif //OPENPSTD_PSTDFILE_H
