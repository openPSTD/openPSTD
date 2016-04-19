//
// Created by michiel on 21-3-2016.
//

#include "PSTDFileAccess.h"

using namespace boost::filesystem;


namespace OpenPSTD
{
    namespace Shared
    {
        OPENPSTD_SHARED_EXPORT PSTDFileAccess::PSTDFileAccess():
                mutex(std::make_shared<boost::recursive_mutex>())
        {
        }

        OPENPSTD_SHARED_EXPORT PSTDFileAccess::~PSTDFileAccess()
        {
            boost::unique_lock<boost::recursive_mutex> m_lock(*this->mutex);
            this->Close();
        }

        OPENPSTD_SHARED_EXPORT locking_ptr<PSTDFile> PSTDFileAccess::GetDocument()
        {
            return locking_ptr<PSTDFile>(this->d, this->mutex);
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::New(path filename)
        {
            boost::unique_lock<boost::recursive_mutex> m_lock(*this->mutex);
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
            boost::unique_lock<boost::recursive_mutex> m_lock(*this->mutex);
            this->realPath = path(filename.c_str());
            this->d = PSTDFile::Open(this->realPath);
            this->Change();
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::Save()
        {
            boost::unique_lock<boost::recursive_mutex> m_lock(*this->mutex);
            this->d->Commit();
            this->Change();
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::SaveAs(path filename)
        {
            boost::unique_lock<boost::recursive_mutex> m_lock(*this->mutex);
            throw "Not implemented";
        }

        OPENPSTD_SHARED_EXPORT void PSTDFileAccess::Close()
        {
            boost::unique_lock<boost::recursive_mutex> m_lock(*this->mutex);
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
            boost::unique_lock<boost::recursive_mutex> m_lock(*this->mutex);
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
            boost::unique_lock<boost::recursive_mutex> m_lock(*this->mutex);
            if(this->d)
                return true;
            else
                return false;
        }

        OPENPSTD_SHARED_EXPORT std::string PSTDFileAccess::GetFilename()
        {
            boost::unique_lock<boost::recursive_mutex> m_lock(*this->mutex);
            std::string result = this->realPath.filename().string();
            return result;
        }


        OPENPSTD_SHARED_EXPORT const char *PSTDFileAccessInUseException::what() const noexcept
        {
            return "The PSTD file is still in use to execute this operation";
        }
    }
}



