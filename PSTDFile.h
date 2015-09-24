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
// Purpose:
//
//
//////////////////////////////////////////////////////////////////////////

//
// Created by michiel on 18-7-2015.
//

#ifndef OPENPSTD_PSTDFILE_H
#define OPENPSTD_PSTDFILE_H

#include <string>
#include <unqlite.h>
#include "rapidjson/document.h"
#include <memory>
#include <vector>
#include "GeneralTypes.h"
#include "InvalidationData.h"
#include <QVector2D>
#include <QVector3D>

using PSTDFile_Key_t = std::shared_ptr<std::vector<char> >;

class PSTDFileSettings
{
private:
    float calctime;
    float c1;
    float ampMax;
    float rho;
    float patcherror;
    float tfactRK;
    float gridSpacing;
    double band_width;
    int PMLCells;
    int SaveNth;
public:
    float GetGridSpacing();
    void SetGridSpacing(float value);
    float GetPatchError();
    void SetPatchError(float value);
    float GetRenderTime();
    void SetRenderTime(float value);
    int GetPMLCells();
    void SetPMLCells(int value);
    float GetAttenuationOfPMLCells();
    void SetAttenuationOfPMLCells(float value);
    float GetDensityOfAir();
    void SetDensityOfAir(float value);
    float GetSoundSpeed();
    void SetSoundSpeed(float value);
    float GetFactRK();
    void SetFactRK(float value);
    int GetSaveNth();
    void SetSaveNth(int value);
    double getBandWidth();
    void setBandWidth(double value);
};

class DomainEdge
{
    float Absorption;
    bool LR;
};

class Domain
{
public:
    QVector2D TopLeft;
    QVector2D Size;
    DomainEdge T, L, B, R;
};

class PSTDFileConfiguration
{
public:
    PSTDFileSettings Settings;
    std::vector<QVector3D> Speakers;
    std::vector<QVector3D> Receivers;
    std::vector<Domain> Domains;
};

enum PSTD_DOMAIN_SIDE
{
    PSTD_DOMAIN_SIDE_TOP,
    PSTD_DOMAIN_SIDE_BOTTOM,
    PSTD_DOMAIN_SIDE_LEFT,
    PSTD_DOMAIN_SIDE_RIGHT,
};

std::string DomainSideToString(PSTD_DOMAIN_SIDE side);

class PSTDFile: public InvalidationData
{
private:
    bool changed;
    std::shared_ptr<rapidjson::Document> sceneConf;
    std::unique_ptr<unqlite, int(*)(unqlite*)> backend;

    std::unique_ptr<std::string> GetStringValue(PSTDFile_Key_t key);
    char* GetRawValue(PSTDFile_Key_t key, unqlite_int64* nBytes);

    void SetStringValue(PSTDFile_Key_t key, std::shared_ptr<std::string> value);
    void SetRawValue(PSTDFile_Key_t key, unqlite_int64 nBytes, const char *value);
    void DeleteValue(PSTDFile_Key_t key);

    void initilize();
    unsigned int IncrementFrameCount(unsigned int domain);

    PSTDFile_Key_t CreateKey(unsigned int prefix, std::initializer_list<unsigned int> list);
    std::string KeyToString(PSTDFile_Key_t key);

    template<typename T>
    T GetValue(PSTDFile_Key_t key)
    {
        std::unique_ptr<unqlite_int64> nBytes;
        int rc;

        std::unique_ptr<T> zBuf;//Dynamically allocated buffer

        rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), zBuf.get(), nBytes.get());
        if( rc != UNQLITE_OK )
        {
            //todo throw error exception
        }

        return *zBuf;
    }

    template<typename T>
    void SetValue(PSTDFile_Key_t key, T value)
    {
        int rc;

        rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), &value, sizeof(T));

        if( rc != UNQLITE_OK )
        {
            //todo throw error exception
        }
    }

public:
    static std::unique_ptr<PSTDFile> Open(const std::string &filename);
    static std::unique_ptr<PSTDFile> New(const std::string &filename);

    PSTDFile();
    void Commit();

    std::shared_ptr<rapidjson::Document> GetSceneConf();
    void SetSceneConf(std::shared_ptr<rapidjson::Document> scene);

    std::shared_ptr<rapidjson::Document> GetPSTDConf();
    void SetPSTDConf(std::shared_ptr<rapidjson::Document> PSTD);

    PSTDFileSettings GetSettings();
    void SetSettings(PSTDFileSettings value);

    int GetDomainCount();
    int GetFrameCount(unsigned int domain);

    PSTD_FRAME GetFrame(unsigned int frame, unsigned int domain);
    void SaveNextFrame(unsigned int domain, PSTD_FRAME frame);

    void InitilizeSimulationResults(int domains);
    void DeleteSimulationResults();
};


#endif //OPENPSTD_PSTDFILE_H
