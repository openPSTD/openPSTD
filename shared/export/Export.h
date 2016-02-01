//
// Created by michiel on 24-1-2016.
//

#ifndef OPENPSTD_EXPORT_H
#define OPENPSTD_EXPORT_H

#include <shared/PSTDFile.h>
#include <string>
#include <vector>

namespace OpenPSTD
{
    namespace Shared
    {
        /**
        * Possible export implementations:
        * Images (image/png, image/bmp, image/jpg)
        * HDF5 (application/x-hdf)
        * Wav (audio/wav)
        */

        class ExportFormatNotSupported : public std::exception
        {
        private:
            std::string _format;
            std::vector<std::string> _supportedFormats;
        public:
            /**
             * Built exception for format errors
             */
            ExportFormatNotSupported(std::string format, std::vector<std::string> supportedFormats);

            const char *what() const noexcept override;
        };

    }
}

#endif //OPENPSTD_EXPORT_H
