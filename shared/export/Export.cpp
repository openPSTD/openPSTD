//
// Created by michiel on 24-1-2016.
//

#include "Image.h"


namespace OpenPSTD
{
    namespace Shared
    {

        OPENPSTD_SHARED_EXPORT ExportFormatNotSupported::ExportFormatNotSupported(std::string format,
                                                           std::vector<std::string> supportedFormats) :
                _format(format), _supportedFormats(supportedFormats)
        {

        }

        OPENPSTD_SHARED_EXPORT const char *ExportFormatNotSupported::what() const noexcept
        {
            if (_supportedFormats.size() == 0)
            {
                return ("Format " + _format + " not supported").c_str();
            }
            else
            {
                std::string s = _supportedFormats[0];
                for (int i = 1; i < _supportedFormats.size(); ++i)
                {
                    s = s + ", " + _supportedFormats[i];
                }
                return ("Format " + _format + " not supported, supported formats are: " + s).c_str();
            }
        }

    }
}