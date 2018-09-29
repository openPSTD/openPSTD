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
#include <iostream>

extern "C"
{
#include <unqlite.h>
}

#include <boost/lexical_cast.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/serialization/vector.hpp>

namespace OpenPSTD
{
    namespace Shared
    {
        using namespace std;

#define PSTD_FILE_VERSION 4

#define PSTD_FILE_PREFIX_SCENE 1

#define PSTD_FILE_PREFIX_RESULTS_SCENE 100
#define PSTD_FILE_PREFIX_RESULTS_SCENE_KERNEL 101
#define PSTD_FILE_PREFIX_RESULTS_FRAME_COUNT 102
#define PSTD_FILE_PREFIX_RESULTS_FRAMEDATA 103
#define PSTD_FILE_PREFIX_RESULTS_RECEIVERDATA 104

#define PSTD_FILE_PREFIX_VERSION 10000

        std::string PSTDFileKeyToString(PSTDFile_Key_t key)
        {
            unsigned int *values = (unsigned int *) key->data();

            std::string result("{" + boost::lexical_cast<std::string>(values[0]) + ",[");
            for (int i = 1; i < key->size() / 4; i++)
            {
                result = result + boost::lexical_cast<std::string>(values[i]);
                if (i < key->size() / 4 - 1)
                {
                    result = result + ",";
                }
            }
            result = result + "]}";

            return result;
        }

        OPENPSTD_SHARED_EXPORT PSTDFileVersionException::PSTDFileVersionException(int fileVersion)
        {
            this->FileVersion = fileVersion;
        }

        OPENPSTD_SHARED_EXPORT const char *PSTDFileVersionException::what() const noexcept
        {

            std::string str = ("Wrong version: " + boost::lexical_cast<std::string>(FileVersion) + "(expected: " +
                    boost::lexical_cast<std::string>(PSTD_FILE_VERSION) + ")");
            char* c_str = new char[str.length()];
            strcpy(c_str, str.data());
            return c_str;
        }

        OPENPSTD_SHARED_EXPORT PSTDFileIOException::PSTDFileIOException(int unqlite_error, PSTDFile_Key_t key, std::string action)
        {
            _unqlite_error = unqlite_error;
            _key = key;
            _action = action;
        }

        std::string PSTDFileIOException::GetErrorString(int unqlite_error) const noexcept
        {
            switch (unqlite_error)
            {
                case UNQLITE_OK: return "Successful result";
                case UNQLITE_NOMEM: return "Out of memory";
                case UNQLITE_ABORT: return "Another thread have released this instance";
                case UNQLITE_IOERR: return "IO error";
                case UNQLITE_CORRUPT: return "Corrupt pointer";
                case UNQLITE_LOCKED: return "Forbidden Operation";
                case UNQLITE_BUSY: return "The database file is locked";
                case UNQLITE_DONE: return "Operation done";
                case UNQLITE_PERM: return "Permission error";
                case UNQLITE_NOTIMPLEMENTED: return "Method not implemented by the underlying Key/Value storage engine";
                case UNQLITE_NOTFOUND: return "No such record";
                case UNQLITE_NOOP: return "No such method";
                case UNQLITE_INVALID: return "Invalid parameter";
                case UNQLITE_EOF: return "End Of Input";
                case UNQLITE_UNKNOWN: return "Unknown configuration option";
                case UNQLITE_LIMIT: return "Database limit reached";
                case UNQLITE_EXISTS: return "Record exists";
                case UNQLITE_EMPTY: return "Empty record";
                case UNQLITE_COMPILE_ERR: return "Compilation error";
                case UNQLITE_VM_ERR: return "Virtual machine error";
                case UNQLITE_FULL: return "Full database (unlikely)";
                case UNQLITE_CANTOPEN: return "Unable to open the database file";
                case UNQLITE_READ_ONLY: return "Read only Key/Value storage engine";
                case SXERR_ORANGE: return "Out of range value";
                case SXERR_MORE: return "Need more input";
                case SXERR_SYNTAX: return "Syntax error";
                case SXERR_OVERFLOW: return "Stack or buffer overflow";
                case SXERR_WILLBLOCK: return "Operation will block";
                case SXERR_FORMAT: return "Invalid format";
                case SXERR_NEXT: return "Not an error";
                case SXERR_OS: return "System call return an error";
                case SXERR_CONTINUE: return "Not an error: Operation in progress";
                case SXERR_NOMATCH: return "No match";
                case SXERR_RESET: return "Operation reset";
                case SXERR_SHORT: return "Buffer too short";
                case SXERR_PATH: return "Path error";
                case SXERR_TIMEOUT: return "Timeout";
                case SXERR_BIG: return "Too big for processing";
                case SXERR_RETRY: return "Retry your call";
                case SXERR_IGNORE: return "Ignore";
                default: return "UNKOWN ERROR";
            }
        }

        OPENPSTD_SHARED_EXPORT const char *PSTDFileIOException::what() const noexcept
        {
            if(this->_key)
            {
                return ("Error with '" + _action + "': error " + this->GetErrorString(_unqlite_error) + " (" +
                        boost::lexical_cast<std::string>(_unqlite_error) +
                        ") with key " + PSTDFileKeyToString(_key)).c_str();
            }
            else
            {
                return ("Error with '" + _action + "': error " + this->GetErrorString(_unqlite_error) + " (" +
                        boost::lexical_cast<std::string>(_unqlite_error) + ")").c_str();
            }
        }

        OPENPSTD_SHARED_EXPORT std::unique_ptr<PSTDFile> PSTDFile::Open(const boost::filesystem::path &path)
        {
            std::string filename = path.string();
            std::unique_ptr<PSTDFile> result = std::unique_ptr<PSTDFile>(new PSTDFile());
            unqlite *backend;
            unqlite_open(&backend, filename.c_str(), UNQLITE_OPEN_CREATE);
            unqlite_config(backend, UNQLITE_CONFIG_DISABLE_AUTO_COMMIT);//commits are done by the save function
            result->backend = std::unique_ptr<unqlite, int (*)(unqlite *)>(backend, unqlite_close);
            int version = result->GetValue<int>(PSTDFile::CreateKey(PSTD_FILE_PREFIX_VERSION, {}));
            if (version != PSTD_FILE_VERSION)
            {
                throw PSTDFileVersionException(version);
            }
            return result;
        }

        OPENPSTD_SHARED_EXPORT std::unique_ptr<PSTDFile> PSTDFile::New(const boost::filesystem::path &path)
        {
            std::string filename = path.string();
            std::unique_ptr<PSTDFile> result(new PSTDFile());

            //create db
            unqlite *backend;
            unqlite_open(&backend, filename.c_str(), UNQLITE_OPEN_CREATE);
            unqlite_config(backend, UNQLITE_CONFIG_DISABLE_AUTO_COMMIT);//commits are done by the save function
            result->backend = std::unique_ptr<unqlite, int (*)(unqlite *)>(backend, unqlite_close);

            //add version
            result->SetValue<int>(result->CreateKey(PSTD_FILE_PREFIX_VERSION, {}), PSTD_FILE_VERSION);

            //create basic geometry with default options
            result->SetSceneConf(Kernel::PSTDConfiguration::CreateDefaultConf());

            //create empty geometry for results
            result->SetSceneConf(result->CreateKey(PSTD_FILE_PREFIX_RESULTS_SCENE, {}), Kernel::PSTDConfiguration::CreateEmptyConf());

            result->Commit();
            return result;
        }

        OPENPSTD_SHARED_EXPORT PSTDFile::PSTDFile() : backend(nullptr, unqlite_close)
        {

        }

        OPENPSTD_SHARED_EXPORT shared_ptr<Kernel::PSTDConfiguration> PSTDFile::GetSceneConf()
        {
            return this->GetSceneConf(CreateKey(PSTD_FILE_PREFIX_SCENE, {}));
        }

        OPENPSTD_SHARED_EXPORT void PSTDFile::SetSceneConf(shared_ptr<Kernel::PSTDConfiguration> scene)
        {
            this->SetSceneConf(CreateKey(PSTD_FILE_PREFIX_SCENE, {}), scene);
        }

        OPENPSTD_SHARED_EXPORT int PSTDFile::GetResultsDomainCount()
        {
            std::shared_ptr<Kernel::PSTDConfiguration> conf = this->GetResultsSceneConf();
            return conf->Domains.size();
        }

        OPENPSTD_SHARED_EXPORT int PSTDFile::GetResultsFrameCount(unsigned int domain)
        {
            return GetValue<int>(CreateKey(PSTD_FILE_PREFIX_RESULTS_FRAME_COUNT, {domain}));
        }

        OPENPSTD_SHARED_EXPORT Kernel::PSTD_FRAME_PTR PSTDFile::GetResultsFrame(unsigned int frame, unsigned int domain)
        {
            unqlite_int64 size;
            float *result = (float *) this->GetRawValue(CreateKey(PSTD_FILE_PREFIX_RESULTS_FRAMEDATA, {domain, frame}), &size);
            return make_shared<Kernel::PSTD_FRAME>(result, result + (size / 4));
        }

        OPENPSTD_SHARED_EXPORT void PSTDFile::SaveNextResultsFrame(unsigned int domain, Kernel::PSTD_FRAME_PTR frameData)
        {
            unsigned int frame = IncrementFrameCount(domain);
            this->SetRawValue(CreateKey(PSTD_FILE_PREFIX_RESULTS_FRAMEDATA, {domain, frame}),
                              frameData->size() * sizeof(Kernel::PSTD_FRAME_UNIT), frameData->data());
        }

        OPENPSTD_SHARED_EXPORT void PSTDFile::InitializeResults()
        {
            auto conf = GetSceneConf();
            SetSceneConf(CreateKey(PSTD_FILE_PREFIX_RESULTS_SCENE, {}), conf);
            for (unsigned int i = 0; i < conf->Domains.size(); i++)
            {
                SetValue<int>(CreateKey(PSTD_FILE_PREFIX_RESULTS_FRAME_COUNT, {i}), 0);
            }

            for(unsigned int i = 0; i < conf->Receivers.size(); i++)
            {
                //create empty records for the receivers
                SetRawValue(CreateKey(PSTD_FILE_PREFIX_RESULTS_RECEIVERDATA, {i}), 0, nullptr);
            }
        }

        OPENPSTD_SHARED_EXPORT void PSTDFile::DeleteResults()
        {
            int rc;

            int domainCount = this->GetResultsDomainCount();
            for (unsigned int d = 0; d < domainCount; d++)
            {
                int frameCount = this->GetResultsFrameCount(d);
                for (unsigned int f = 0; f < frameCount; f++)
                {
                    this->DeleteValue(CreateKey(PSTD_FILE_PREFIX_RESULTS_FRAMEDATA, {d, f}));
                }
                this->DeleteValue(CreateKey(PSTD_FILE_PREFIX_RESULTS_FRAME_COUNT, {d}));
            }
            this->SetSceneConf(CreateKey(PSTD_FILE_PREFIX_RESULTS_SCENE, {}), Kernel::PSTDConfiguration::CreateEmptyConf());
        }

        OPENPSTD_SHARED_EXPORT void PSTDFile::OutputDebugInfo()
        {
            boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
            int rc = 0;
            unqlite_kv_cursor *cursor;
            unqlite_kv_cursor_init(this->backend.get(), &cursor);

            unqlite_kv_cursor_first_entry(cursor);
            while (unqlite_kv_cursor_valid_entry(cursor))
            {
                int nbytes;
                char *buffer;

                unqlite_kv_cursor_key(cursor, NULL, &nbytes);
                buffer = new char[nbytes];
                unqlite_kv_cursor_key(cursor, buffer, &nbytes);

                std::cout << "Key " << PSTDFileKeyToString(CreateKeyFromData(buffer, nbytes)) << std::endl;

                delete[] buffer;

                unqlite_kv_cursor_next_entry(cursor);
            }

            unqlite_kv_cursor_release(this->backend.get(), cursor);
        }

        std::unique_ptr<std::string> PSTDFile::GetStringValue(PSTDFile_Key_t key)
        {
            boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
            unqlite_int64 nBytes;  //Data length
            char *zBuf;     //Dynamically allocated buffer

            zBuf = GetRawValue(key, &nBytes);

            unique_ptr<std::string> result = unique_ptr<std::string>(new std::string(zBuf));

            delete zBuf;

            return result;
        }

        char *PSTDFile::GetRawValue(PSTDFile_Key_t key, unqlite_int64 *nBytes)
        {
            boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
            int rc;
            char *zBuf;     //Dynamically allocated buffer

            *nBytes = 0;

            rc = unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), NULL, nBytes);
            if (rc != UNQLITE_OK)
            {
                throw PSTDFileIOException(rc, key, "fetch data");
            }

            zBuf = new char[*nBytes];

            unqlite_kv_fetch(this->backend.get(), key->data(), key->size(), zBuf, nBytes);

            return zBuf;
        }

        PSTDFile_Key_t PSTDFile::CreateKey(unsigned int prefix, std::initializer_list<unsigned int> list)
        {
            //the length of the list times int
            int length = sizeof(int) * (list.size() + 1);
            char *data = new char[length];

            //cast to integer for easy access
            int *key = (int *) data;

            //fill the key
            key[0] = prefix;

            int i = 0;
            for (auto elem : list)
            {
                key[i + 1] = elem;
                i++;
            }

            PSTDFile_Key_t result = make_shared<std::vector<char>>(data, data + length);
            delete[] data;

            return result;
        }

        PSTDFile_Key_t PSTDFile::CreateKeyFromData(char *pBuf, int pnByte)
        {
            PSTDFile_Key_t result = make_shared<std::vector<char>>(pBuf, pBuf + pnByte);
            return result;
        }

        void PSTDFile::SetStringValue(PSTDFile_Key_t key, std::shared_ptr<std::string> value)
        {
            //add the 1 for the null terminator :)
            SetRawValue(key, value->length() + 1, value->c_str());
        }

        void PSTDFile::SetRawValue(PSTDFile_Key_t key, unqlite_int64 nBytes, const void *value)
        {
            boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
            int rc;

            rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), value, nBytes);

            if (rc != UNQLITE_OK)
            {
                throw PSTDFileIOException(rc, key, "store data");
            }
        }

        void PSTDFile::AppendRawValue(PSTDFile_Key_t key, unqlite_int64 nBytes, const void *value)
        {
            boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
            int rc;

            rc = unqlite_kv_append(this->backend.get(), key->data(), key->size(), value, nBytes);

            if (rc != UNQLITE_OK)
            {
                throw PSTDFileIOException(rc, key, "append data");
            }
        }

        OPENPSTD_SHARED_EXPORT std::shared_ptr<Kernel::PSTDConfiguration> PSTDFile::GetSceneConf(PSTDFile_Key_t key)
        {
            namespace io = boost::iostreams;
            try
            {
                typedef std::vector<char> buffer_type;

                Kernel::PSTDConfiguration data_in;
                unqlite_int64 nBytes;
                auto data = this->GetRawValue(key, &nBytes);//database

                io::basic_array_source<char> source(data, nBytes);
                io::stream<io::basic_array_source<char> > input_stream(source);
                boost::archive::text_iarchive ia(input_stream);

                ia >> data_in;

                delete[] data;

                std::shared_ptr<Kernel::PSTDConfiguration> result = make_shared<Kernel::PSTDConfiguration>(data_in);
                return result;
            }
            catch(boost::archive::archive_exception e)
            {
                std::cout << "P1: " << e.what() << std::endl;
                throw e;
            }
        }

        OPENPSTD_SHARED_EXPORT void PSTDFile::SetSceneConf(PSTDFile_Key_t key, std::shared_ptr<Kernel::PSTDConfiguration> scene)
        {
            try
            {
                namespace io = boost::iostreams;
                typedef std::vector<char> buffer_type;
                buffer_type buffer;

                io::stream<io::back_insert_device<buffer_type> > output_stream(buffer);
                boost::archive::text_oarchive oa(output_stream);

                oa << (*scene);
                output_stream.flush();
                this->SetRawValue(key, buffer.size(), buffer.data());
            }
            catch(boost::archive::archive_exception e)
            {
                std::cout << "P2: " << e.what() << std::endl;
                throw e;
            }
        }

        unsigned int PSTDFile::IncrementFrameCount(unsigned int domain)
        {
            auto key = CreateKey(PSTD_FILE_PREFIX_RESULTS_FRAME_COUNT, {domain});
            int frame = GetValue<int>(key);
            SetValue<int>(key, frame + 1);
            return frame;
        }

        void PSTDFile::DeleteValue(PSTDFile_Key_t key)
        {
            boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
            int rc;

            rc = unqlite_kv_delete(this->backend.get(), key->data(), key->size());

            if (rc != UNQLITE_OK)
            {
                throw PSTDFileIOException(rc, key, "delete data");
            }
        }

        OPENPSTD_SHARED_EXPORT std::shared_ptr<Kernel::PSTDConfiguration> PSTDFile::GetResultsSceneConf()
        {
            return this->GetSceneConf(CreateKey(PSTD_FILE_PREFIX_RESULTS_SCENE, {}));
        }

        OPENPSTD_SHARED_EXPORT void PSTDFile::SaveReceiverData(unsigned int receiver, Kernel::PSTD_RECEIVER_DATA_PTR data)
        {
            this->AppendRawValue(CreateKey(PSTD_FILE_PREFIX_RESULTS_RECEIVERDATA, {receiver}),
                                 data->size() * sizeof(Kernel::PSTD_FRAME_UNIT), data->data());
        }

        OPENPSTD_SHARED_EXPORT Kernel::PSTD_RECEIVER_DATA_PTR PSTDFile::GetReceiverData(unsigned int receiver)
        {
            unqlite_int64 size;
            float *result = (float *) this->GetRawValue(CreateKey(PSTD_FILE_PREFIX_RESULTS_RECEIVERDATA, {receiver}), &size);
            return make_shared<Kernel::PSTD_RECEIVER_DATA>(result, result + (size / 4));
        }

        int PSTDFile::GetResultsReceiverCount()
        {
            std::shared_ptr<Kernel::PSTDConfiguration> conf = this->GetResultsSceneConf();
            return conf->Receivers.size();
        }

        void PSTDFile::Commit()
        {
            boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
            int rc = unqlite_commit(this->backend.get());

            if(rc != UNQLITE_OK)
                throw PSTDFileIOException(rc, nullptr, "Commit");
        }

        void PSTDFile::Rollback()
        {
            boost::unique_lock<boost::recursive_mutex> lock(this->backendMutex);
            int rc = unqlite_rollback(this->backend.get());

            if(rc != UNQLITE_OK)
                throw PSTDFileIOException(rc, nullptr, "Commit");
        }


    }
}
