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
//      18-7-2015
//
// Authors:
//      Michiel Fortuin
//      Omar Richardson
//      Louis van Harten
//
// Purpose:
//      Contains the settings and parameters needed to perform the
//      complete openPSTD calculation.
//
//////////////////////////////////////////////////////////////////////////

#ifndef OPENPSTD_PSTDFILE_H
#define OPENPSTD_PSTDFILE_H

#include <string>
#include <Eigen/Dense>

extern "C"
{
#include <unqlite.h>
}

#include <memory>
#include <vector>
#include "GeneralTypes.h"
#include "InvalidationData.h"
#include <QVector2D>
#include <QVector3D>

using PSTDFile_Key_t = std::shared_ptr<std::vector<char> >;

std::string PSTDFileKeyToString(PSTDFile_Key_t key);

class PSTDFileSettings {
private:
    float calctime;
    float c1;
    float ampMax;
    float freqMax;
    float rho;
    float patcherror;
    float tfactRK;
    float gridSpacing;
    float band_width;
    float wave_length;
    std::vector<float> rk_coefficients;
    bool spectral_interpolation;
    int PMLCells;
    int SaveNth;
    bool gpu;
    bool multithread;
    Eigen::ArrayXf window;
public:
    float GetGridSpacing();
    void SetGridSpacing(float value);

    float GetPatchError();
    void SetPatchError(float value);

    int GetWindowSize();

    float GetRenderTime();
    void SetRenderTime(float value);

    int GetPMLCells();
    void SetPMLCells(int value);

    float GetAttenuationOfPMLCells();
    void SetAttenuationOfPMLCells(float value);

    float GetDensityOfAir();
    void SetDensityOfAir(float value);

    float GetMaxFrequency();
    void SetMaxFrequency(float value);

    float GetSoundSpeed();
    void SetSoundSpeed(float value);

    float GetFactRK();
    void SetFactRK(float value);

    int GetSaveNth();
    void SetSaveNth(int value);

    float GetBandWidth();
    void SetBandWidth(float value);

    bool GetSpectralInterpolation();
    void SetSpectralInterpolation(bool value);

    float GetWaveLength();
    void SetWaveLength(float value);

    float GetTimeStep();

    bool GetGPUAccel();
    void SetGPUAccel(bool value);

    bool GetMultiThread();
    void SetMultiThread(bool value);

    std::vector<float> GetRKCoefficients();
    void SetRKCoefficients(std::vector<float> coef);

    Eigen::ArrayXf GetWindow();
    void SetWindow(Eigen::ArrayXf A);
};

class DomainEdge {
public:
    float Absorption;
    bool LR;
};

class Domain {
public:
    QVector2D TopLeft;
    QVector2D Size;
    DomainEdge T, L, B, R;
};

class PSTDFileConfiguration {
public:
    PSTDFileSettings Settings;
    std::vector<QVector3D> Speakers;
    std::vector<QVector3D> Receivers;
    std::vector<Domain> Domains;
};

enum PSTD_DOMAIN_SIDE {
    PSTD_DOMAIN_SIDE_TOP,
    PSTD_DOMAIN_SIDE_BOTTOM,
    PSTD_DOMAIN_SIDE_LEFT,
    PSTD_DOMAIN_SIDE_RIGHT,
};


class PSTDFileVersionException : public std::exception
{
private:
    int FileVersion;

public:
    PSTDFileVersionException(int fileVersion);
    const char* what() const noexcept override;
};

class PSTDFileIOException : public std::exception
{
private:
    int _unqlite_error;
    PSTDFile_Key_t _key;
    std::string _action;

public:
    PSTDFileIOException(int unqlite_error, PSTDFile_Key_t key, std::string action);
    const char* what() const noexcept override;
};

std::string DomainSideToString(PSTD_DOMAIN_SIDE side);

class PSTDFile : public InvalidationData
{
private:
    std::unique_ptr<unqlite, int (*)(unqlite *)> backend;

    std::unique_ptr<std::string> GetStringValue(PSTDFile_Key_t key);

    char *GetRawValue(PSTDFile_Key_t key, unqlite_int64 *nBytes);

    void SetStringValue(PSTDFile_Key_t key, std::shared_ptr<std::string> value);

    void SetRawValue(PSTDFile_Key_t key, unqlite_int64 nBytes, const void *value);

    void DeleteValue(PSTDFile_Key_t key);

    void initialize();

    unsigned int IncrementFrameCount(unsigned int domain);

    static PSTDFile_Key_t CreateKey(unsigned int prefix, std::initializer_list<unsigned int> list);
    static PSTDFile_Key_t CreateKeyFromData(char *pBuf,int pnByte);

    template<typename T>
    T GetValue(PSTDFile_Key_t key)
    {
        unqlite_int64 nBytes;
        int rc;

        T zBuf;

        rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), &zBuf, &nBytes);
        if (rc != UNQLITE_OK) {
            throw PSTDFileIOException(rc, key, "fetch data");
        }

        return zBuf;
    }

    template<typename T>
    void SetValue(PSTDFile_Key_t key, T value)
    {
        int rc;

        rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), &value, sizeof(T));

        if (rc != UNQLITE_OK) {
            throw PSTDFileIOException(rc, key, "fetch data");
        }
    }

    template<typename T>
    std::vector<T> GetArray(PSTDFile_Key_t key)
    {
        unqlite_int64 nBytes;
        unqlite_int64 n;
        int rc;

        rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), NULL, &nBytes);
        if (rc != UNQLITE_OK) {
            throw PSTDFileIOException(rc, key, "fetch size");
        }

        n = nBytes/sizeof(T);

        std::vector<T> zBuf(n);

        rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), zBuf.data(), &nBytes);
        if (rc != UNQLITE_OK) {
            throw PSTDFileIOException(rc, key, "fetch data");
        }

        return zBuf;
    }

    template<typename T>
    void SetArray(PSTDFile_Key_t key, const std::vector<T>& value)
    {
        int rc;

        rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), value.data(), value.size()*sizeof(T));
        if (rc != UNQLITE_OK) {
            throw PSTDFileIOException(rc, key, "store data");
        }
    }

public:

    /**
     * Creates a default configuration
     */
    static std::shared_ptr<PSTDFileConfiguration> CreateDefaultConf();
    /**
     * Opens a file
     * @param filename the filename that has to be opened
     * @return a unique ptr to the PSTD file
     */
    static std::unique_ptr<PSTDFile> Open(const std::string &filename);

    /**
     * Creates a new file, a file can't exist only in memory, so a filename has to be given
     * @param filename The filename of the newly create PSTD file
     * @return A unique ptr to the new PSTD file
     */
    static std::unique_ptr<PSTDFile> New(const std::string &filename);

    PSTDFile();

    /**
     * Reads the scene config out of the file
     * @return a shared ptr to a new object of scene configuration
     */
    std::shared_ptr<PSTDFileConfiguration> GetSceneConf();

    /**
     * Writes the scene config to the file
     * @param scene a shared ptr to an object of scene configuration
     */
    void SetSceneConf(std::shared_ptr<PSTDFileConfiguration> scene);

    /**
     * Reads the scene config out of the file
     * @return a shared ptr to a new object of scene configuration
     */
    //todo fix for the correct type
    std::shared_ptr<PSTDFileConfiguration> GetPSTDConf();

    /**
     * Writes the scene config to the file
     * @param scene a shared ptr to an object of scene configuration
     */
    //todo fix for the correct type
    void SetPSTDConf(std::shared_ptr<PSTDFileConfiguration> scene);

    int GetDomainCount();

    int GetFrameCount(unsigned int domain);

    PSTD_FRAME_PTR GetFrame(unsigned int frame, unsigned int domain);

    void SaveNextFrame(unsigned int domain, PSTD_FRAME_PTR frame);

    void InitializeSimulationResults(int domains);

    void DeleteSimulationResults();

    void OutputDebugInfo();
};


#endif //OPENPSTD_PSTDFILE_H
