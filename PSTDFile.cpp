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

#include "PSTDFile.h"
extern "C" 
{
#include <unqlite.h>
}
#include <iostream>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <boost/lexical_cast.hpp>

using namespace std;

const char* PSTDEmptyScene = "{\n"
        "    \"domains\": [\n"
        "        {\n"
        "            \"topleft\": [\n"
        "                0.0,\n"
        "                0.0\n"
        "            ],\n"
        "            \"size\": [\n"
        "                10.0,\n"
        "                10.0\n"
        "            ],\n"
        "            \"edges\": {\n"
        "                \"l\": {\n"
        "                    \"a\": 0.0,\n"
        "                    \"lr\": false\n"
        "                },\n"
        "                \"b\": {\n"
        "                    \"a\": 0.0,\n"
        "                    \"lr\": false\n"
        "                },\n"
        "                \"t\": {\n"
        "                    \"a\": 0.0,\n"
        "                    \"lr\": false\n"
        "                },\n"
        "                \"r\": {\n"
        "                    \"a\": 0.0,\n"
        "                    \"lr\": false\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "    ],\n"
        "    \"calctime\": 1.0,\n"
        "    \"c1\": 340.0,\n"
        "    \"ampmax\": 20000.0,\n"
        "    \"receivers\": [[6.0, 5.0, 0.0]],\n"
        "    \"rho\": 1.2000000476837158,\n"
        "    \"plotdir\": \"./openPSTD_plots_20140903_1009\",\n"
        "    \"patcherror\": 70.0,\n"
        "    \"speakers\": [\n"
        "        [\n"
        "            4.0,\n"
        "            5.0,\n"
        "            0.0\n"
        "        ]\n"
        "    ],\n"
        "    \"tfactRK\": 0.5,\n"
        "    \"grid_spacing\": 0.20000000298023224,\n"
        "    \"PMLcells\": 50,"
        "    \"SaveNth\": 1\n"
        "}";

#define PSTD_FILE_PREFIX_SCENE 1
#define PSTD_FILE_PREFIX_PSTD 2
#define PSTD_FILE_PREFIX_DOMAIN_COUNT 3
#define PSTD_FILE_PREFIX_FRAME_COUNT 4
#define PSTD_FILE_PREFIX_FRAMEDATA 5


std::unique_ptr<PSTDFile> PSTDFile::Open(const std::string &filename)
{
    std::unique_ptr<PSTDFile> result = std::unique_ptr<PSTDFile>(new PSTDFile());
    result->changed = false;
    unqlite* backend;
    unqlite_open(&backend, filename.c_str(), UNQLITE_OPEN_CREATE);
    result->backend = std::unique_ptr<unqlite, int(*)(unqlite*)>(backend, unqlite_close);
    result->initilize();
    return result;
}

std::unique_ptr<PSTDFile> PSTDFile::New(const std::string &filename)
{
    {
        std::unique_ptr<PSTDFile> result(new PSTDFile());

        unqlite *backend;
        unqlite_open(&backend, filename.c_str(), UNQLITE_OPEN_CREATE);
        result->backend = std::unique_ptr<unqlite, int(*)(unqlite*)>(backend, unqlite_close);

        std::shared_ptr<rapidjson::Document> SceneConf(new rapidjson::Document());
        SceneConf->Parse(PSTDEmptyScene);
        result->SetSceneConf(SceneConf);
        result->Commit();

        std::shared_ptr<rapidjson::Document> PSTDConf(new rapidjson::Document());
        PSTDConf->Parse("{}");
        result->SetPSTDConf(PSTDConf);

        result->SetValue<int>(result->CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}), 0);
    }

    return PSTDFile::Open(filename);
}

PSTDFile::PSTDFile(): backend(nullptr, unqlite_close)
{

}

std::shared_ptr<rapidjson::Document> PSTDFile::GetSceneConf()
{
    return this->sceneConf;
}

void PSTDFile::SetSceneConf(std::shared_ptr<rapidjson::Document> scene)
{
    this->sceneConf = scene;
    this->changed = true;
}

std::shared_ptr<rapidjson::Document> PSTDFile::GetPSTDConf()
{
    std::unique_ptr<std::string> json = this->GetStringValue(CreateKey(PSTD_FILE_PREFIX_PSTD, {}));
    std::shared_ptr<rapidjson::Document> document(new rapidjson::Document());
    document->Parse(json->c_str());
    return document;
}

void PSTDFile::SetPSTDConf(std::shared_ptr<rapidjson::Document> PSTD)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    PSTD->Accept(writer);
    shared_ptr<string> json = make_shared<string>(buffer.GetString());
    this->SetStringValue(CreateKey(PSTD_FILE_PREFIX_PSTD, {}), json);
}

int PSTDFile::GetDomainCount()
{
    return GetValue<int>(CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}));
}

int PSTDFile::GetFrameCount(unsigned int domain)
{
    return GetValue<int>(CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {domain}));
}

PSTD_FRAME PSTDFile::GetFrame(unsigned int frame, unsigned int domain)
{
    unqlite_int64 size;
    char* result = this->GetRawValue(CreateKey(PSTD_FILE_PREFIX_FRAMEDATA, {domain, frame}), &size);

    return shared_ptr<vector<char>>(new vector<char>(result, result+size));
}

void PSTDFile::SaveNextFrame(unsigned int domain, PSTD_FRAME frameData)
{
    unsigned int frame = IncrementFrameCount(domain);
    this->SetRawValue(CreateKey(PSTD_FILE_PREFIX_FRAMEDATA, {domain, frame}), frameData->size(), frameData->data());
}

void PSTDFile::InitilizeSimulationResults(int domains)
{
    SetValue<int>(CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}), domains);
    for(unsigned int i = 0; i < domains; i++)
    {
        SetValue<int>(CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {i}), 0);
    }
}

void PSTDFile::DeleteSimulationResults()
{
    int rc;

    int domainCount = this->GetDomainCount();
    for(unsigned int d = 0; d < domainCount; d++)
    {
        int frameCount = this->GetFrameCount(d);
        for(unsigned int f = 0; f < frameCount; f++)
        {
            this->DeleteValue(CreateKey(PSTD_FILE_PREFIX_FRAMEDATA, {d, f}));
        }
        this->DeleteValue(CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {d}));
    }
    SetValue<int>(CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}), 0);
}

void PSTDFile::Commit()
{
    if(this->changed)
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        this->sceneConf->Accept(writer);
        shared_ptr<string> json = make_shared<string>(buffer.GetString());
        this->SetStringValue(CreateKey(PSTD_FILE_PREFIX_SCENE, {}), json);
    }
    this->changed = false;
}

std::unique_ptr<std::string> PSTDFile::GetStringValue(PSTDFile_Key_t key)
{
    unqlite_int64 nBytes;  //Data length
    char *zBuf;     //Dynamically allocated buffer

    zBuf = GetRawValue(key, &nBytes);

    unique_ptr<std::string> result = unique_ptr<std::string>(new std::string(zBuf));

    delete zBuf;

    return result;
}

char *PSTDFile::GetRawValue(PSTDFile_Key_t key, unqlite_int64* nBytes)
{
    int rc;
    char *zBuf;     //Dynamically allocated buffer

    rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), NULL, nBytes);
    if( rc != UNQLITE_OK )
    {
        std::cout << "Error: " << rc;
        std::cout << " for key: " << this->KeyToString(key) << std::endl;
        //todo throw error exception
    }

    zBuf = new char[*nBytes];

    unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), zBuf, nBytes);

    return zBuf;
}

void PSTDFile::initilize()
{
    std::unique_ptr<std::string> json(this->GetStringValue(CreateKey(PSTD_FILE_PREFIX_SCENE, {})));
    std::shared_ptr<rapidjson::Document> result(new rapidjson::Document());
    result->Parse(json->c_str());
    this->sceneConf = result;

    changed = false;
}

PSTDFile_Key_t PSTDFile::CreateKey(unsigned int prefix, std::initializer_list<unsigned int> list)
{
    //the length of the list times int
    int length = sizeof(int)*(list.size()+1);
    char* data = new char[length];

    //cast to integer for easy access
    int* key = (int*)data;

    //fill the key
    key[0] = prefix;

    int i = 0;
    for( auto elem : list )
    {
        key[i+1] = elem;
        i++;
    }

    PSTDFile_Key_t result = shared_ptr<std::vector<char> >(new vector<char>(data, data+length));
    delete[] data;

    return result;
}

std::string PSTDFile::KeyToString(PSTDFile_Key_t key)
{
    unsigned int * values = (unsigned int*)key->data();

    std::string result("{" + boost::lexical_cast<std::string>(values[0]) + ",[");
    for(int i = 1; i < key->size()/4; i++)
    {
        result = result + boost::lexical_cast<std::string>(values[i]);
        if(i < key->size()/4-1)
        {
            result = result + ",";
        }
    }
    result = result + "]}";

    return result;
}

void PSTDFile::SetStringValue(PSTDFile_Key_t key, std::shared_ptr<std::string> value)
{
    //add the 1 for the null terminator :)
    SetRawValue(key, value->length()+1, value->c_str());
}

void PSTDFile::SetRawValue(PSTDFile_Key_t key, unqlite_int64 nBytes, const char *value)
{
    int rc;

    rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), value, nBytes);

    if( rc != UNQLITE_OK )
    {
        //todo throw error exception
    }
}

unsigned int PSTDFile::IncrementFrameCount(unsigned int domain)
{
    auto key = CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {domain});
    int frame = GetValue<int>(key);
    SetValue<int>(key, frame+1);
    return frame;
}

void PSTDFile::DeleteValue(PSTDFile_Key_t key)
{
    int rc;

    rc = unqlite_kv_delete(this->backend.get(), key->data(), key->size());

    if( rc != UNQLITE_OK )
    {
        //todo throw error exception
    }
}


std::string DomainSideToString(PSTD_DOMAIN_SIDE side)
{
    switch(side) {
        case PSTD_DOMAIN_SIDE_TOP   : return "t";
        case PSTD_DOMAIN_SIDE_BOTTOM : return "b";
        case PSTD_DOMAIN_SIDE_LEFT  : return "l";
        case PSTD_DOMAIN_SIDE_RIGHT  : return "r";
    }
}

PSTDFileSettings PSTDFile::GetSettings()
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = this->GetSceneConf();

    PSTDFileSettings settings;
    settings.SetGridSpacing((*conf)["grid_spacing"].GetDouble());
    settings.SetPatchError((*conf)["patcherror"].GetDouble());
    settings.SetRenderTime((*conf)["calctime"].GetDouble());
    settings.SetPMLCells((*conf)["PMLcells"].GetInt());
    settings.SetAttenuationOfPMLCells((*conf)["ampmax"].GetDouble());
    settings.SetDensityOfAir((*conf)["rho"].GetDouble());
    settings.SetSoundSpeed((*conf)["c1"].GetDouble());
    settings.SetFactRK((*conf)["tfactRK"].GetDouble());
    settings.SetSaveNth((*conf)["SaveNth"].GetInt());

    return settings;
}
void PSTDFile::SetSettings(PSTDFileSettings settings)
{
    using namespace rapidjson;

    std::shared_ptr<Document> conf = this->GetSceneConf();

    (*conf)["grid_spacing"] = settings.GetGridSpacing();
    (*conf)["patcherror"] = settings.GetPatchError();
    (*conf)["calctime"] = settings.GetRenderTime();
    (*conf)["PMLcells"] = settings.GetPMLCells();
    (*conf)["ampmax"] = settings.GetAttenuationOfPMLCells();
    (*conf)["rho"] = settings.GetDensityOfAir();
    (*conf)["c1"] = settings.GetSoundSpeed();
    (*conf)["tfactRK"] = settings.GetFactRK();
    (*conf)["SaveNth"] = settings.GetSaveNth();

    this->SetSceneConf(conf);
}

float PSTDFileSettings::GetGridSpacing()
{
    return this->gridSpacing;
}
void PSTDFileSettings::SetGridSpacing(float value)
{
    this->gridSpacing = value;
    //TODO edit max frequency here as well
}
float PSTDFileSettings::GetPatchError()
{
    return this->patcherror;
}
void PSTDFileSettings::SetPatchError(float value)
{
    this->patcherror = value;
}
float PSTDFileSettings::GetRenderTime()
{
    return this->calctime;
}
void PSTDFileSettings::SetRenderTime(float value)
{
    this->calctime = value;
}
int PSTDFileSettings::GetPMLCells()
{
    return this->PMLCells;
}
void PSTDFileSettings::SetPMLCells(int value)
{
    this->PMLCells = value;
}
float PSTDFileSettings::GetAttenuationOfPMLCells()
{
    return this->ampMax;
}
void PSTDFileSettings::SetAttenuationOfPMLCells(float value)
{
    this->ampMax = value;
}
float PSTDFileSettings::GetDensityOfAir()
{
    return this->rho;
}
void PSTDFileSettings::SetDensityOfAir(float value)
{
    this->rho = value;
}
/**
 * TODO: add dynamic behavior to setters of freqMax and grid spacing
 */
float PSTDFileSettings::GetMaxFrequency()
{
    return this->freqMax;
}
void PSTDFileSettings::SetMaxFrequency(float value)
{
    this->freqMax = value;
    //TODO edit grid spacing as well
}
float PSTDFileSettings::GetSoundSpeed()
{
    return this->c1;
}
void PSTDFileSettings::SetSoundSpeed(float value)
{
    this->c1 = value;
}
float PSTDFileSettings::GetFactRK()
{
    return this->tfactRK;
}
void PSTDFileSettings::SetFactRK(float value)
{
    this->tfactRK = value;
}
int PSTDFileSettings::GetSaveNth()
{
    return this->SaveNth;
}
void PSTDFileSettings::SetSaveNth(int value)
{
    this->SaveNth = value;
}

float PSTDFileSettings::getBandWidth() {
    return this->band_width;
}

void PSTDFileSettings::setBandWidth(float value) {
    this->band_width = value;
}

float PSTDFileSettings::getWaveLength() {
    return this->wave_length;
}

void PSTDFileSettings::setWaveLength(float value) {
    this->wave_length = value;
}

bool PSTDFileSettings::getSpectralInterpolation() {
    return this->spectral_interpolation;
}

void PSTDFileSettings::setSpectralInterpolation(bool value) {
    this->spectral_interpolation = value;
}

int PSTDFileSettings::GetWindowSize() {
    //directly translated from original Python implementation
    int tmp = std::round((this->patcherror*0.7-17)/2.);
    tmp = std::round(tmp*2);
    return tmp;
}
