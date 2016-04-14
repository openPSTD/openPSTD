//
// Created by michiel on 21-3-2016.
//

#include "PSTDFileAccess.h"

using namespace boost::filesystem;


namespace OpenPSTD
{
    namespace Shared
    {
        OPENPSTD_SHARED_EXPORT PSTDFileAccess::~PSTDFileAccess()
        {
            this->Close();
        }

        OPENPSTD_SHARED_EXPORT std::shared_ptr<PSTDFile> PSTDFileAccess::GetDocument()
        {
            return this->d;
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::New(path filename)
        {
            this->realPath = path(filename.c_str());
            path tempFilename = this->GetTempPath();

            if(exists(this->realPath))
            {
                //todo handle this with an exception
                remove(this->realPath);
            }
            // create file
            {
                PSTDFile::New(this->realPath);
            } //make sure the destructor is called

            if(exists(tempFilename))
            {
                //todo handle this with an exception
                remove(tempFilename);
            }
            //copy the file
            copy_file(this->realPath, tempFilename);

            //opens the temporary file
            this->InternalOpen();

            this->Change();
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::Open(path filename)
        {
            this->realPath = path(filename.c_str());
            path tempFilename = this->GetTempPath();

            // copy file
            copy_file(this->realPath, tempFilename);

            // open file
            this->InternalOpen();

            this->Change();
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::Save()
        {
            this->InternalClose();
            path tempFilename = this->GetTempPath();

            if(exists(this->realPath))
            {
                remove(this->realPath);
            }
            // copy file
            copy_file(tempFilename, this->realPath);
            //todo: compress
            this->InternalOpen();
            this->Change();
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::SaveAs(path filename)
        {
            this->InternalClose();
            path tempFilename = this->GetTempPath();
            copy_file(tempFilename, filename);
            this->Open(filename);
            //todo: compress
            this->InternalOpen();
            this->Change();
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::Close()
        {
            if(this->d)
            {
                this->InternalClose();
                remove(GetTempPath());

                this->Change();
            }
        }

        OPENPSTD_SHARED_NO_EXPORT path PSTDFileAccess::GetTempPath()
        {
            path s = this->realPath.string() + "-temp";
            return s;
        }

        OPENPSTD_SHARED_EXPORT bool PSTDFileAccess::IsChanged()
        {
            if(this->d)
            {
                return InvalidationData::IsChanged() || this->d->IsChanged();
            }
            else
            {
                return InvalidationData::IsChanged();
            }
        }

        OPENPSTD_SHARED_NO_EXPORT void PSTDFileAccess::InternalClose()
        {
            if(this->d.use_count() == 1)
            {
                this->d = nullptr;
            }
            else
            {
                throw PSTDFileAccessInUseException();
            }
        }

        OPENPSTD_SHARED_NO_EXPORT void PSTDFileAccess::InternalOpen()
        {
            this->d = PSTDFile::Open(this->GetTempPath());
        }

        OPENPSTD_SHARED_EXPORT bool PSTDFileAccess::IsDocumentLoaded()
        {
            if(this->d)
                return true;
            else
                return false;
        }

        OPENPSTD_SHARED_EXPORT std::string PSTDFileAccess::GetFilename()
        {
            std::string result = this->realPath.filename().string();
            return result;
        }


        OPENPSTD_SHARED_EXPORT const char *PSTDFileAccessInUseException::what() const noexcept
        {
            return "The PSTD file is still in use to execute this operation";
        }
    }
}



