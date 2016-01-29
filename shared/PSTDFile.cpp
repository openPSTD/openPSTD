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

namespace OpenPSTD
{
    namespace Shared
    {
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

        PSTDFileVersionException::PSTDFileVersionException(int fileVersion)
        {
            this->FileVersion = fileVersion;
        }

        const char *PSTDFileVersionException::what() const noexcept
        {
            return ("Wrong version: " + boost::lexical_cast<std::string>(FileVersion) + "(expected: " +
                    boost::lexical_cast<std::string>(PSTD_FILE_VERSION) + ")").c_str();
        }

        PSTDFileIOException::PSTDFileIOException(int unqlite_error, PSTDFile_Key_t key, std::string action)
        {
            _unqlite_error = unqlite_error;
            _key = key;
            _action = action;
        }

        const char *PSTDFileIOException::what() const noexcept
        {
            return ("Error with '" + _action + "': error code: " + boost::lexical_cast<std::string>(_unqlite_error) +
                    " with key " + PSTDFileKeyToString(_key) + ")").c_str();
        }

        std::unique_ptr<PSTDFile> PSTDFile::Open(const std::string &filename)
        {
            std::unique_ptr<PSTDFile> result = std::unique_ptr<PSTDFile>(new PSTDFile());
            unqlite *backend;
            unqlite_open(&backend, filename.c_str(), UNQLITE_OPEN_CREATE);
            result->backend = std::unique_ptr<unqlite, int (*)(unqlite *)>(backend, unqlite_close);
            int version = result->GetValue<int>(PSTDFile::CreateKey(PSTD_FILE_PREFIX_VERSION, {}));
            if (version != PSTD_FILE_VERSION)
            {
                throw PSTDFileVersionException(version);
            }
            return result;
        }

        std::unique_ptr<PSTDFile> PSTDFile::New(const std::string &filename)
        {
            {
                std::unique_ptr<PSTDFile> result(new PSTDFile());

                //create db
                unqlite *backend;
                unqlite_open(&backend, filename.c_str(), UNQLITE_OPEN_CREATE);
                result->backend = std::unique_ptr<unqlite, int (*)(unqlite *)>(backend, unqlite_close);

                //add version
                result->SetValue<int>(result->CreateKey(PSTD_FILE_PREFIX_VERSION, {}), PSTD_FILE_VERSION);

                //create basic geometry with default options
                std::shared_ptr<Kernel::PSTDConfiguration> SceneConf = Kernel::PSTDConfiguration::CreateDefaultConf();
                result->SetSceneConf(SceneConf);

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

        PSTDFile::PSTDFile() : backend(nullptr, unqlite_close)
        {

        }

        shared_ptr<Kernel::PSTDConfiguration> PSTDFile::GetSceneConf()
        {
            shared_ptr<Kernel::PSTDConfiguration> conf = make_shared<Kernel::PSTDConfiguration>();
            conf->Settings = this->GetValue<Kernel::PSTDSettings>(this->CreateKey(PSTD_FILE_PREFIX_SCENE_SETTINGS, {}));
            conf->Speakers = this->GetArray<QVector3D>(this->CreateKey(PSTD_FILE_PREFIX_SCENE_SPEAKERS, {}));
            conf->Receivers = this->GetArray<QVector3D>(this->CreateKey(PSTD_FILE_PREFIX_SCENE_RECEIVERS, {}));
            conf->Domains = this->GetArray<Kernel::DomainConf>(this->CreateKey(PSTD_FILE_PREFIX_SCENE_DOMAINS, {}));

            return conf;
        }

        void PSTDFile::SetSceneConf(shared_ptr<Kernel::PSTDConfiguration> scene)
        {
            this->SetValue(this->CreateKey(PSTD_FILE_PREFIX_SCENE_SETTINGS, {}), scene->Settings);
            this->SetArray(this->CreateKey(PSTD_FILE_PREFIX_SCENE_SPEAKERS, {}), scene->Speakers);
            this->SetArray(this->CreateKey(PSTD_FILE_PREFIX_SCENE_RECEIVERS, {}), scene->Receivers);
            this->SetArray(this->CreateKey(PSTD_FILE_PREFIX_SCENE_DOMAINS, {}), scene->Domains);
        }

        int PSTDFile::GetDomainCount()
        {
            return GetValue<int>(CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}));
        }

        int PSTDFile::GetFrameCount(unsigned int domain)
        {
            return GetValue<int>(CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {domain}));
        }

        Kernel::PSTD_FRAME_PTR PSTDFile::GetFrame(unsigned int frame, unsigned int domain)
        {
            unqlite_int64 size;
            float *result = (float *) this->GetRawValue(CreateKey(PSTD_FILE_PREFIX_FRAMEDATA, {domain, frame}), &size);
            return shared_ptr<Kernel::PSTD_FRAME>(new Kernel::PSTD_FRAME(result, result + (size / 4)));
        }

        void PSTDFile::SaveNextFrame(unsigned int domain, Kernel::PSTD_FRAME_PTR frameData)
        {
            unsigned int frame = IncrementFrameCount(domain);
            this->SetRawValue(CreateKey(PSTD_FILE_PREFIX_FRAMEDATA, {domain, frame}),
                              frameData->size() * sizeof(Kernel::PSTD_FRAME_UNIT), frameData->data());
        }

        void PSTDFile::InitializeSimulationResults(int domains)
        {
            SetValue<int>(CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}), domains);
            for (unsigned int i = 0; i < domains; i++)
            {
                SetValue<int>(CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {i}), 0);
            }
        }

        void PSTDFile::DeleteSimulationResults()
        {
            int rc;

            int domainCount = this->GetDomainCount();
            for (unsigned int d = 0; d < domainCount; d++)
            {
                int frameCount = this->GetFrameCount(d);
                for (unsigned int f = 0; f < frameCount; f++)
                {
                    this->DeleteValue(CreateKey(PSTD_FILE_PREFIX_FRAMEDATA, {d, f}));
                }
                this->DeleteValue(CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {d}));
            }
            SetValue<int>(CreateKey(PSTD_FILE_PREFIX_DOMAIN_COUNT, {}), 0);
        }

        void PSTDFile::OutputDebugInfo()
        {
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
            unqlite_int64 nBytes;  //Data length
            char *zBuf;     //Dynamically allocated buffer

            zBuf = GetRawValue(key, &nBytes);

            unique_ptr<std::string> result = unique_ptr<std::string>(new std::string(zBuf));

            delete zBuf;

            return result;
        }

        char *PSTDFile::GetRawValue(PSTDFile_Key_t key, unqlite_int64 *nBytes)
        {
            int rc;
            char *zBuf;     //Dynamically allocated buffer

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
            //PSTDFile_Key_t result = std::shared_ptr<std::vector<char>>(new vector<char>(pBuf, pBuf + pnByte));
            return result;
        }

        void PSTDFile::SetStringValue(PSTDFile_Key_t key, std::shared_ptr<std::string> value)
        {
            //add the 1 for the null terminator :)
            SetRawValue(key, value->length() + 1, value->c_str());
        }

        void PSTDFile::SetRawValue(PSTDFile_Key_t key, unqlite_int64 nBytes, const void *value)
        {
            int rc;

            rc = unqlite_kv_store(this->backend.get(), key->data(), key->size(), value, nBytes);

            if (rc != UNQLITE_OK)
            {
                throw PSTDFileIOException(rc, key, "store data");
            }
        }

        unsigned int PSTDFile::IncrementFrameCount(unsigned int domain)
        {
            auto key = CreateKey(PSTD_FILE_PREFIX_FRAME_COUNT, {domain});
            int frame = GetValue<int>(key);
            SetValue<int>(key, frame + 1);
            return frame;
        }

        void PSTDFile::DeleteValue(PSTDFile_Key_t key)
        {
            int rc;

            rc = unqlite_kv_delete(this->backend.get(), key->data(), key->size());

            if (rc != UNQLITE_OK)
            {
                throw PSTDFileIOException(rc, key, "delete data");
            }
        }

    }
}
