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

            if(exists(this->realPath))
            {
                remove(this->realPath);
            }
            this->d = PSTDFile::New(this->realPath);
            this->Change();
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::Open(path filename)
        {
            this->realPath = path(filename.c_str());
            this->d = PSTDFile::Open(this->realPath);
            this->Change();
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::Save()
        {
            this->d->Commit();
            this->Change();
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::SaveAs(path filename)
        {
            throw "Not implemented";
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::Close()
        {
            if(this->d)
            {
                if(this->d.use_count() == 1)
                {
                    this->d = nullptr;
                }
                else
                {
                    throw PSTDFileAccessInUseException();
                }
                this->Change();
            }
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



