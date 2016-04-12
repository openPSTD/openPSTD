//
// Created by michiel on 21-3-2016.
//

#ifndef OPENPSTD_PSTDFILEACCESS_H
#define OPENPSTD_PSTDFILEACCESS_H

#include "openpstd-shared_export.h"
#include "PSTDFile.h"
#include <memory>
#include <string>
#include <boost/filesystem.hpp>

namespace OpenPSTD
{
    namespace Shared
    {
        class OPENPSTD_SHARED_EXPORT PSTDFileAccessInUseException : public std::exception
        {
        public:
            OPENPSTD_SHARED_EXPORT const char *what() const noexcept override;
        };

        class OPENPSTD_SHARED_EXPORT PSTDFileAccess: public InvalidationData
        {
        private:
            std::shared_ptr<PSTDFile> d;
            boost::filesystem::path realPath;

            OPENPSTD_SHARED_NO_EXPORT boost::filesystem::path GetTempPath();

            OPENPSTD_SHARED_NO_EXPORT void InternalClose();
            OPENPSTD_SHARED_NO_EXPORT void InternalOpen();

        public:
            OPENPSTD_SHARED_EXPORT ~PSTDFileAccess();

            OPENPSTD_SHARED_EXPORT std::shared_ptr<PSTDFile> GetDocument();

            OPENPSTD_SHARED_EXPORT void New(boost::filesystem::path filename);

            OPENPSTD_SHARED_EXPORT void Open(boost::filesystem::path filename);

            OPENPSTD_SHARED_EXPORT void Save();

            OPENPSTD_SHARED_EXPORT void SaveAs(boost::filesystem::path filename);

            OPENPSTD_SHARED_EXPORT void Close();

            OPENPSTD_SHARED_EXPORT virtual bool IsChanged() override;
        };
    }
}

#endif //OPENPSTD_PSTDFILEACCESS_H
