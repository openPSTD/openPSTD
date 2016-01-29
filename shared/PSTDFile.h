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
#include <kernel/GeneralTypes.h>
#include <kernel/KernelInterface.h>
#include <shared/InvalidationData.h>
#include <QVector2D>
#include <QVector3D>

namespace OpenPSTD
{
    namespace Shared
    {
        using PSTDFile_Key_t = std::shared_ptr<std::vector<char> >;

        std::string PSTDFileKeyToString(PSTDFile_Key_t key);

        class PSTDFileVersionException : public std::exception
        {
        private:
            int FileVersion;

        public:
            PSTDFileVersionException(int fileVersion);

            const char *what() const noexcept override;
        };

        class PSTDFileIOException : public std::exception
        {
        private:
            int _unqlite_error;
            PSTDFile_Key_t _key;
            std::string _action;

        public:
            PSTDFileIOException(int unqlite_error, PSTDFile_Key_t key, std::string action);

            const char *what() const noexcept override;
        };

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

            static PSTDFile_Key_t CreateKeyFromData(char *pBuf, int pnByte);

            template<typename T>
            T GetValue(PSTDFile_Key_t key)
            {
                unqlite_int64 nBytes;
                int rc;

                T zBuf;

                rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), &zBuf, &nBytes);
                if (rc != UNQLITE_OK)
                {
                    throw PSTDFileIOException(rc, key, "fetch data");
                }

                return zBuf;
            }

            template<typename T>
            void SetValue(PSTDFile_Key_t key, T value)
            {
                int rc;

                rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), &value, sizeof(T));

                if (rc != UNQLITE_OK)
                {
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
                if (rc != UNQLITE_OK)
                {
                    throw PSTDFileIOException(rc, key, "fetch size");
                }

                n = nBytes / sizeof(T);

                std::vector<T> zBuf(n);

                rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), zBuf.data(), &nBytes);
                if (rc != UNQLITE_OK)
                {
                    throw PSTDFileIOException(rc, key, "fetch data");
                }

                return zBuf;
            }

            template<typename T>
            void SetArray(PSTDFile_Key_t key, const std::vector<T> &value)
            {
                int rc;

                rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), value.data(),
                                      value.size() * sizeof(T));
                if (rc != UNQLITE_OK)
                {
                    throw PSTDFileIOException(rc, key, "store data");
                }
            }

        public:
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
            std::shared_ptr<Kernel::PSTDConfiguration> GetSceneConf();

            /**
             * Writes the scene config to the file
             * @param scene a shared ptr to an object of scene configuration
             */
            void SetSceneConf(std::shared_ptr<Kernel::PSTDConfiguration> scene);

            int GetDomainCount();

            int GetFrameCount(unsigned int domain);

            Kernel::PSTD_FRAME_PTR GetFrame(unsigned int frame, unsigned int domain);

            void SaveNextFrame(unsigned int domain, Kernel::PSTD_FRAME_PTR frame);

            void InitializeSimulationResults(int domains);

            void DeleteSimulationResults();

            void OutputDebugInfo();
        };

    }
}

#endif //OPENPSTD_PSTDFILE_H
