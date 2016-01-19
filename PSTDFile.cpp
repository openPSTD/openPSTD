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
// Date: 19-1-2016
//
//
// Authors: M. R. Fortuin
//
//
//////////////////////////////////////////////////////////////////////////

#include "PSTDFile.h"

extern "C"
{
#include <unqlite.h>
}

#include <iostream>
#include <boost/lexical_cast.hpp>

using namespace std;

#define PSTD_FILE_PREFIX_SCENE 1
#define PSTD_FILE_PREFIX_PSTD 2
#define PSTD_FILE_PREFIX_DOMAIN_COUNT 3
#define PSTD_FILE_PREFIX_FRAME_COUNT 4
#define PSTD_FILE_PREFIX_FRAMEDATA 5
#define PSTD_FILE_PREFIX_SCENE_SETTINGS 6
#define PSTD_FILE_PREFIX_SCENE_SPEAKERS 7
#define PSTD_FILE_PREFIX_SCENE_RECEIVERS 8
#define PSTD_FILE_PREFIX_SCENE_DOMAINS 9
#define PSTD_FILE_PREFIX_VERSION 10000
#define PSTD_FILE_VERSION 1

std::string PSTDFileKeyToString(PSTDFile_Key_t key) {
    unsigned int *values = (unsigned int *) key->data();

    std::string result("{" + boost::lexical_cast<std::string>(values[0]) + ",[");
    for (int i = 1; i < key->size() / 4; i++) {
        result = result + boost::lexical_cast<std::string>(values[i]);
        if (i < key->size() / 4 - 1) {
            result = result + ",";
        }
    }
    result = result + "]}";

    return result;
}

PSTDFileVersionException::PSTDFileVersionException(int fileVersion)
{
    this->FileVersion = fileVersion;
}

const char* PSTDFileVersionException::what() const noexcept
{
    return ("Wrong version: " + boost::lexical_cast<std::string>(FileVersion) + "(expected: " + boost::lexical_cast<std::string>(PSTD_FILE_VERSION) + ")").c_str();
}

PSTDFileIOException::PSTDFileIOException(int unqlite_error, PSTDFile_Key_t key, std::string action)
{
    _unqlite_error = unqlite_error;
    _key = key;
    _action = action;
}

const char* PSTDFileIOException::what() const noexcept
{
    return ("Error with '" + _action + "': error code: " + boost::lexical_cast<std::string>(_unqlite_error) + " with key " + PSTDFileKeyToString(_key) + ")").c_str();
}

std::unique_ptr<PSTDFile> PSTDFile::Open(const std::string &filename) {
    std::unique_ptr<PSTDFile> result = std::unique_ptr<PSTDFile>(new PSTDFile());
    unqlite *backend;
    unqlite_open(&backend, filename.c_str(), UNQLITE_OPEN_CREATE);
    result->backend = std::unique_ptr<unqlite, int (*)(unqlite *)>(backend, unqlite_close);
    int version = result->GetValue<int>(PSTDFile::CreateKey(PSTD_FILE_PREFIX_VERSION, {}));
    if(version != PSTD_FILE_VERSION)
    {
        throw PSTDFileVersionException(version);
    }
    return result;
}

std::unique_ptr<PSTDFile> PSTDFile::New(const std::string &filename) {
    {
        std::unique_ptr<PSTDFile> result(new PSTDFile());

        //create db
        unqlite *backend;
        unqlite_open(&backend, filename.c_str(), UNQLITE_OPEN_CREATE);
        result->backend = std::unique_ptr<unqlite, int (*)(unqlite *)>(backend, unqlite_close);

        //add version
        result->SetValue<int>(result->CreateKey(PSTD_FILE_PREFIX_VERSION, {}), PSTD_FILE_VERSION);

        //create basic geometry with default options
        std::shared_ptr<PSTDFileConfiguration> SceneConf = PSTDFile::CreateDefaultConf();
        result->SetSceneConf2(SceneConf);

        //create PSTD conf
        //todo fix the correct type
        //std::shared_ptr<rapidjson::Document> PSTDConf(new rapidjson::Document());
        //PSTDConf->Parse("{}");
        //result->SetPSTDConf(PSTDConf);

        //zero frames are saves
        result->SetValue<int>(result->CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}), 0);
    }

    return PSTDFile::Open(filename);
}

PSTDFile::PSTDFile() : backend(nullptr, unqlite_close) {

}

shared_ptr<PSTDFileConfiguration> PSTDFile::GetSceneConf2()
{
    shared_ptr<PSTDFileConfiguration> conf = shared_ptr<PSTDFileConfiguration>(new PSTDFileConfiguration());
    conf->Settings = this->GetValue<PSTDFileSettings>(this->CreateKey(PSTD_FILE_PREFIX_SCENE_SETTINGS, {}));
    conf->Speakers = this->GetArray<QVector3D>(this->CreateKey(PSTD_FILE_PREFIX_SCENE_SPEAKERS, {}));
    conf->Receivers = this->GetArray<QVector3D>(this->CreateKey(PSTD_FILE_PREFIX_SCENE_RECEIVERS, {}));
    conf->Domains = this->GetArray<Domain>(this->CreateKey(PSTD_FILE_PREFIX_SCENE_DOMAINS, {}));

    return conf;
}

void PSTDFile::SetSceneConf2(shared_ptr<PSTDFileConfiguration> scene)
{
    this->SetValue(this->CreateKey(PSTD_FILE_PREFIX_SCENE_SETTINGS, {}), scene->Settings);
    this->SetArray(this->CreateKey(PSTD_FILE_PREFIX_SCENE_SPEAKERS, {}), scene->Speakers);
    this->SetArray(this->CreateKey(PSTD_FILE_PREFIX_SCENE_RECEIVERS, {}), scene->Receivers);
    this->SetArray(this->CreateKey(PSTD_FILE_PREFIX_SCENE_DOMAINS, {}), scene->Domains);
}

shared_ptr<PSTDFileConfiguration> PSTDFile::GetPSTDConf2()
{
    shared_ptr<PSTDFileConfiguration> conf = shared_ptr<PSTDFileConfiguration>(new PSTDFileConfiguration());

    return conf;
}

void PSTDFile::SetPSTDConf2(shared_ptr<PSTDFileConfiguration> scene)
{

}

int PSTDFile::GetDomainCount() {
    return GetValue<int>(CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}));
}

int PSTDFile::GetFrameCount(unsigned int domain) {
    return GetValue<int>(CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {domain}));
}

PSTD_FRAME_PTR PSTDFile::GetFrame(unsigned int frame, unsigned int domain) {
    unqlite_int64 size;
    char *result = this->GetRawValue(CreateKey(PSTD_FILE_PREFIX_FRAMEDATA, {domain, frame}), &size);
    return shared_ptr<PSTD_FRAME>(new PSTD_FRAME(result, result + size));
}

void PSTDFile::SaveNextFrame(unsigned int domain, PSTD_FRAME_PTR frameData) {
    unsigned int frame = IncrementFrameCount(domain);
    this->SetRawValue(CreateKey(PSTD_FILE_PREFIX_FRAMEDATA, {domain, frame}), frameData->size()*sizeof(PSTD_FRAME_UNIT), frameData->data());
}

void PSTDFile::InitializeSimulationResults(int domains) {
    SetValue<int>(CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}), domains);
    for (unsigned int i = 0; i < domains; i++) {
        SetValue<int>(CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {i}), 0);
    }
}

void PSTDFile::DeleteSimulationResults() {
    int rc;

    int domainCount = this->GetDomainCount();
    for (unsigned int d = 0; d < domainCount; d++) {
        int frameCount = this->GetFrameCount(d);
        for (unsigned int f = 0; f < frameCount; f++) {
            this->DeleteValue(CreateKey(PSTD_FILE_PREFIX_FRAMEDATA, {d, f}));
        }
        this->DeleteValue(CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {d}));
    }
    SetValue<int>(CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}), 0);
}

std::unique_ptr<std::string> PSTDFile::GetStringValue(PSTDFile_Key_t key) {
    unqlite_int64 nBytes;  //Data length
    char *zBuf;     //Dynamically allocated buffer

    zBuf = GetRawValue(key, &nBytes);

    unique_ptr<std::string> result = unique_ptr<std::string>(new std::string(zBuf));

    delete zBuf;

    return result;
}

char *PSTDFile::GetRawValue(PSTDFile_Key_t key, unqlite_int64 *nBytes) {
    int rc;
    char *zBuf;     //Dynamically allocated buffer

    rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), NULL, nBytes);
    if (rc != UNQLITE_OK) {
        throw PSTDFileIOException(rc, key, "fetch data");
    }

    zBuf = new char[*nBytes];

    unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), zBuf, nBytes);

    return zBuf;
}

PSTDFile_Key_t PSTDFile::CreateKey(unsigned int prefix, std::initializer_list<unsigned int> list) {
    //the length of the list times int
    int length = sizeof(int) * (list.size() + 1);
    char *data = new char[length];

    //cast to integer for easy access
    int *key = (int *) data;

    //fill the key
    key[0] = prefix;

    int i = 0;
    for (auto elem : list) {
        key[i + 1] = elem;
        i++;
    }

    PSTDFile_Key_t result = shared_ptr<std::vector<char> >(new vector<char>(data, data + length));
    delete[] data;

    return result;
}

void PSTDFile::SetStringValue(PSTDFile_Key_t key, std::shared_ptr<std::string> value) {
    //add the 1 for the null terminator :)
    SetRawValue(key, value->length() + 1, value->c_str());
}

void PSTDFile::SetRawValue(PSTDFile_Key_t key, unqlite_int64 nBytes, const void *value) {
    int rc;

    rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), value, nBytes);

    if (rc != UNQLITE_OK) {
        throw PSTDFileIOException(rc, key, "store data");
    }
}

unsigned int PSTDFile::IncrementFrameCount(unsigned int domain) {
    auto key = CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {domain});
    int frame = GetValue<int>(key);
    SetValue<int>(key, frame + 1);
    return frame;
}

void PSTDFile::DeleteValue(PSTDFile_Key_t key) {
    int rc;

    rc = unqlite_kv_delete(this->backend.get(), key->data(), key->size());

    if (rc != UNQLITE_OK) {
        throw PSTDFileIOException(rc, key, "delete data");
    }
}


std::string DomainSideToString(PSTD_DOMAIN_SIDE side) {
    switch (side) {
        case PSTD_DOMAIN_SIDE_TOP   :
            return "t";
        case PSTD_DOMAIN_SIDE_BOTTOM :
            return "b";
        case PSTD_DOMAIN_SIDE_LEFT  :
            return "l";
        case PSTD_DOMAIN_SIDE_RIGHT  :
            return "r";
    }
}

shared_ptr<PSTDFileConfiguration> PSTDFile::CreateDefaultConf()
{
    shared_ptr<PSTDFileConfiguration> conf = shared_ptr<PSTDFileConfiguration>(new PSTDFileConfiguration());
    conf->Settings.SetRenderTime(1.0f);
    conf->Settings.SetSoundSpeed(340.0f);
    conf->Settings.SetAttenuationOfPMLCells(20000.0f);
    conf->Settings.SetDensityOfAir(1.2f);
    conf->Settings.SetPatchError(70.0f);
    conf->Settings.SetFactRK(0.5f);
    conf->Settings.SetGridSpacing(0.2);
    conf->Settings.SetPMLCells(50);
    conf->Settings.SetSaveNth(1);

    //todo: fix good default values for these parameters
    conf->Settings.SetBandWidth(1.0f);
    conf->Settings.SetRKCoefficients(vector<float>());
    conf->Settings.SetWindow(Eigen::ArrayXf());
    conf->Settings.SetSpectralInterpolation(true);

    conf->Speakers.push_back(QVector3D(4, 5, 0));
    conf->Receivers.push_back(QVector3D(6, 5, 0));

    Domain d;
    d.TopLeft = QVector2D(0, 0);
    d.Size = QVector2D(10, 10);
    d.T.Absorption = 0;
    d.B.Absorption = 0;
    d.L.Absorption = 0;
    d.R.Absorption = 0;

    d.T.LR = false;
    d.B.LR = false;
    d.L.LR = false;
    d.R.LR = false;

    conf->Domains.push_back(d);

    return conf;
}

float PSTDFileSettings::GetGridSpacing() {
    return this->gridSpacing;
}

void PSTDFileSettings::SetGridSpacing(float value) {
    this->gridSpacing = value;
    //TODO edit max frequency here as well
}

float PSTDFileSettings::GetPatchError() {
    return this->patcherror;
}

void PSTDFileSettings::SetPatchError(float value) {
    this->patcherror = value;
}

float PSTDFileSettings::GetRenderTime() {
    return this->calctime;
}

void PSTDFileSettings::SetRenderTime(float value) {
    this->calctime = value;
}

int PSTDFileSettings::GetPMLCells() {
    return this->PMLCells;
}

void PSTDFileSettings::SetPMLCells(int value) {
    this->PMLCells = value;
}

float PSTDFileSettings::GetAttenuationOfPMLCells() {
    return this->ampMax;
}

void PSTDFileSettings::SetAttenuationOfPMLCells(float value) {
    this->ampMax = value;
}

float PSTDFileSettings::GetDensityOfAir() {
    return this->rho;
}

void PSTDFileSettings::SetDensityOfAir(float value) {
    this->rho = value;
}

/**
 * TODO: add dynamic behavior to setters of freqMax and grid spacing
 */
float PSTDFileSettings::GetMaxFrequency() {
    return this->freqMax;
}

void PSTDFileSettings::SetMaxFrequency(float value) {
    this->freqMax = value;
    //TODO edit grid spacing as well
}

float PSTDFileSettings::GetSoundSpeed() {
    return this->c1;
}

void PSTDFileSettings::SetSoundSpeed(float value) {
    this->c1 = value;
}

float PSTDFileSettings::GetFactRK() {
    return this->tfactRK;
}

void PSTDFileSettings::SetFactRK(float value) {
    this->tfactRK = value;
}

int PSTDFileSettings::GetSaveNth() {
    return this->SaveNth;
}

void PSTDFileSettings::SetSaveNth(int value) {
    this->SaveNth = value;
}

float PSTDFileSettings::GetBandWidth() {
    return this->band_width;
}

void PSTDFileSettings::SetBandWidth(float value) {
    this->band_width = value;
}

float PSTDFileSettings::GetWaveLength() {
    return this->wave_length;
}

void PSTDFileSettings::SetWaveLength(float value) {
    this->wave_length = value;
}

bool PSTDFileSettings::GetSpectralInterpolation() {
    return this->spectral_interpolation;
}

void PSTDFileSettings::SetSpectralInterpolation(bool value) {
    this->spectral_interpolation = value;
}

bool PSTDFileSettings::GetGPUAccel() {
    return this->gpu;
}

void PSTDFileSettings::SetGPUAccel(bool value) {
    this->gpu = value;
}

bool PSTDFileSettings::GetMultiThread() {
    return this->multithread;
}

void PSTDFileSettings::SetMultiThread(bool value) {
    this->multithread = value;
}

void PSTDFileSettings::SetWindow(Eigen::ArrayXf A) {
    this->window = A;
}

Eigen::ArrayXf PSTDFileSettings::GetWindow() {
    return this->window;
}

float PSTDFileSettings::GetTimeStep() {
    return this->tfactRK * this->gridSpacing / this->c1;
}

int PSTDFileSettings::GetWindowSize() {
    //directly translated from original Python implementation
    int tmp = std::round((this->patcherror * 0.7 - 17) / 2.);
    tmp = std::round(tmp * 2);
    return tmp;
}

std::vector<float> PSTDFileSettings::GetRKCoefficients() {
    return this->rk_coefficients;
}

void PSTDFileSettings::SetRKCoefficients(std::vector<float> coef) {
    this->rk_coefficients = coef; //Todo: Set somewhere.

}
