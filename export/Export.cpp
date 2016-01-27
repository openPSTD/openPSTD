//
// Created by michiel on 24-1-2016.
//

#include "Export.h"

ExportFormatNotSupported::ExportFormatNotSupported(std::string format, std::vector<std::string> supportedFormats):
    _format(format), _supportedFormats(supportedFormats)
{

}

const char* ExportFormatNotSupported::what() const noexcept
{
    std::string s = _supportedFormats[0];
    for (int i = 1; i < _supportedFormats.size(); ++i)
    {
        s = s + ", " + _supportedFormats[i];
    }
    return ("Format " + _format + " not supported, supported formats are: " + s).c_str();
}

