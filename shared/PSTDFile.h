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

#include "../openpstd-shared_export.h"
#include <string>
#include <Eigen/Dense>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

extern "C"
{
#include "../unqlite/unqlite.h"
}

#include <memory>
#include <vector>
#include "../kernel/GeneralTypes.h"
#include "../kernel/KernelInterface.h"
#include "InvalidationData.h"
#include <QVector2D>
#include <QVector3D>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/serialization.hpp>


/*
 * Non-intrusive serialization functions for QVector2D and QVector3D
 */
namespace boost {
    namespace serialization {

        template<class Archive>
        void save(Archive & ar, const QVector2D & v, unsigned int version)
        {
            float x, y;
            x = v.x();
            y = v.y();
            ar & x;
            ar & y;
        }
        template<class Archive>
        void load(Archive & ar, QVector2D & v, unsigned int version)
        {
            float x, y;
            ar & x;
            ar & y;
            v.setX(x);
            v.setY(y);
        }

        template<class Archive>
        void save(Archive & ar, const QVector3D & v, unsigned int version)
        {
            float x, y, z;
            x = v.x();
            y = v.y();
            z = v.z();
            ar & x;
            ar & y;
            ar & z;
        }

        template<class Archive>
        void load(Archive & ar, QVector3D & v, unsigned int version)
        {
            float x, y, z;
            ar & x;
            ar & y;
            ar & z;
            v.setX(x);
            v.setY(y);
            v.setZ(z);
        }

    } // namespace serialization
} // namespace boost


BOOST_SERIALIZATION_SPLIT_FREE(QVector2D)

BOOST_SERIALIZATION_SPLIT_FREE(QVector3D)

namespace OpenPSTD
{
    namespace Shared
    {
        /**
         * A standard type for the key
         */
        using PSTDFile_Key_t = std::shared_ptr<std::vector<char> >;

        /**
         * Key to string, for debug purposes
         */
        std::string PSTDFileKeyToString(PSTDFile_Key_t key);

        /**
         * Version error of the file
         */
        class OPENPSTD_SHARED_EXPORT PSTDFileVersionException : public std::exception
        {
        private:
            int FileVersion;

        public:
            OPENPSTD_SHARED_EXPORT PSTDFileVersionException(int fileVersion);

            OPENPSTD_SHARED_EXPORT const char *what() const noexcept override;
        };

        /**
         * general exception of unqlite(lowlevel) errors
         */
        class OPENPSTD_SHARED_EXPORT PSTDFileIOException : public std::exception
        {
        private:
            int _unqlite_error;
            PSTDFile_Key_t _key;
            std::string _action;
            std::string GetErrorString(int unqlite_error) const noexcept;
        public:
            OPENPSTD_SHARED_EXPORT PSTDFileIOException(int unqlite_error, PSTDFile_Key_t key, std::string action);

            OPENPSTD_SHARED_EXPORT const char *what() const noexcept override;
        };

        class OPENPSTD_SHARED_EXPORT PSTDFile : public InvalidationData
        {
        private:
            /**
             * Backend, the back end should be automatically closed(by the unique_ptr)
             */
            std::unique_ptr<unqlite, int (*)(unqlite *)> backend;
            bool changed;
            boost::recursive_mutex backendMutex;

            /**
             * Get a value by key as a string
             */
            std::unique_ptr<std::string> GetStringValue(PSTDFile_Key_t key);

            /**
             * Gets the raw value by key(ownership is transfered to caller)
             */
            char *GetRawValue(PSTDFile_Key_t key, unqlite_int64 *nBytes);

            /**
             * Sets a string value by key
             */
            void SetStringValue(PSTDFile_Key_t key, std::shared_ptr<std::string> value);

            /**
             * Set the raw value by key(ownership of value is of the caller)
             */
            void SetRawValue(PSTDFile_Key_t key, unqlite_int64 nBytes, const void *value);

            /**
             * Set the raw value by key(ownership of value is of the caller)
             */
            void AppendRawValue(PSTDFile_Key_t key, unqlite_int64 nBytes, const void *value);

            /**
             * Delete a certain value
             */
            void DeleteValue(PSTDFile_Key_t key);

            /**
             * Increment framecount
             */
            unsigned int IncrementFrameCount(unsigned int domain);

            /**
             * Create key based on a prefix and multiple integer value
             */
            static PSTDFile_Key_t CreateKey(unsigned int prefix, std::initializer_list<unsigned int> list);

            /**
             * Create key based on raw data
             */
            static PSTDFile_Key_t CreateKeyFromData(char *pBuf, int pnByte);

            /**
             * Get a certain value by type T
             */
            template<typename T>
            T GetValue(PSTDFile_Key_t key)
            {
                boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
                unqlite_int64 nBytes = sizeof(T);
                int rc;

                T zBuf;

                rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), &zBuf, &nBytes);
                if (rc != UNQLITE_OK)
                {
                    throw PSTDFileIOException(rc, key, "fetch data");
                }

                return zBuf;
            }

            /**
             * set a certain value by type T
             */
            template<typename T>
            void SetValue(PSTDFile_Key_t key, T value)
            {
                boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
                int rc;

                rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), &value, sizeof(T));

                if (rc != UNQLITE_OK)
                {
                    throw PSTDFileIOException(rc, key, "fetch data");
                }
            }

            /**
             * Get a certain value by type vector<T>
             */
            template<typename T>
            std::vector<T> GetArray(PSTDFile_Key_t key)
            {
                boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
                unqlite_int64 nBytes = 0;
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

            /**
             * Set a certain value by type vector<T>
             */
            template<typename T>
            void SetArray(PSTDFile_Key_t key, const std::vector<T> &value)
            {
                boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
                int rc;

                rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), value.data(),
                                      value.size() * sizeof(T));
                if (rc != UNQLITE_OK)
                {
                    throw PSTDFileIOException(rc, key, "store data");
                }
            }

            /**
             * Reads the scene config out of the file
             * @return a shared ptr to a new object of scene configuration
             */
            std::shared_ptr<Kernel::PSTDConfiguration> GetSceneConf(PSTDFile_Key_t key);

            /**
             * Writes the scene config to the file
             * @param scene a shared ptr to an object of scene configuration
             */
            void SetSceneConf(PSTDFile_Key_t key, std::shared_ptr<Kernel::PSTDConfiguration> scene);

        public:
            /**
             * Opens a file
             * @param filename the filename that has to be opened
             * @return a unique ptr to the PSTD file
             */
            static OPENPSTD_SHARED_EXPORT std::unique_ptr<PSTDFile> Open(const boost::filesystem::path &filename);

            /**
             * Opens a file
             * @param filename the filename that has to be opened
             * @return a unique ptr to the PSTD file
             */
            static OPENPSTD_SHARED_EXPORT std::unique_ptr<PSTDFile> New(const boost::filesystem::path &filename);

            /**
             * Constructs a PSTDFile, for internal use
             */
            OPENPSTD_SHARED_EXPORT PSTDFile();

            /**
             * Compresses the file that so that the file is smaller. This also deletes the data results.
             */
            //todo implement
            //void Compress();


            OPENPSTD_SHARED_EXPORT void Commit();
            OPENPSTD_SHARED_EXPORT void Rollback();


            /**
             * Reads the scene config out of the file
             * @return a shared ptr to a new object of scene configuration
             */
            OPENPSTD_SHARED_EXPORT std::shared_ptr<Kernel::PSTDConfiguration> GetSceneConf();

            /**
             * Writes the scene config to the file
             * @param scene a shared ptr to an object of scene configuration
             */
            OPENPSTD_SHARED_EXPORT void SetSceneConf(std::shared_ptr<Kernel::PSTDConfiguration> scene);



            /**
             * initializes the results(creates domains)
             */
            OPENPSTD_SHARED_EXPORT void InitializeResults();

            /**
             * Reads the scene config out of the file. This scene config is for the results, this config can differs from
             * GetSceneConf.
             * @return a shared ptr to a new object of scene configuration
             */
            OPENPSTD_SHARED_EXPORT std::shared_ptr<Kernel::PSTDConfiguration> GetResultsSceneConf();

            /**
             * Get the number of domains
             */
            OPENPSTD_SHARED_EXPORT int GetResultsDomainCount();

            /**
             * Get the number of frames for a single domain
             * @param domain The domain index
             */
            OPENPSTD_SHARED_EXPORT int GetResultsFrameCount(unsigned int domain);

            /**
             * Gets the data from the frame
             */
            OPENPSTD_SHARED_EXPORT Kernel::PSTD_FRAME_PTR GetResultsFrame(unsigned int frame, unsigned int domain);

            /**
             * Saves the next frame for a certain domain in the file
             */
            OPENPSTD_SHARED_EXPORT void SaveNextResultsFrame(unsigned int domain, Kernel::PSTD_FRAME_PTR frame);

            /**
             * Delete all the simulation results
             */
            OPENPSTD_SHARED_EXPORT void DeleteResults();

            /**
             * Saves a couple of new samples to the receiver data
             */
            OPENPSTD_SHARED_EXPORT void SaveReceiverData(unsigned int receiver, Kernel::PSTD_RECEIVER_DATA_PTR data);

            /**
             * Gets the receivers data from file
             */
            OPENPSTD_SHARED_EXPORT Kernel::PSTD_RECEIVER_DATA_PTR GetReceiverData(unsigned int receiver);

            /**
             * Gets the number of receivers in the results
             */
            OPENPSTD_SHARED_EXPORT int GetResultsReceiverCount();


            /**
             * outputs debug info, only for debug purposes.
             */
            OPENPSTD_SHARED_EXPORT void OutputDebugInfo();
        };

    }
}



#endif //OPENPSTD_PSTDFILE_H
