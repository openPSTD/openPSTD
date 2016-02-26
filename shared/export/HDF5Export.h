//
// Created by michiel on 24-2-2016.
//

#ifndef OPENPSTD_HDF5_H
#define OPENPSTD_HDF5_H

#include <shared/PSTDFile.h>

namespace OpenPSTD
{
    namespace Shared
    {
        class HDF5
        {
        public:
            virtual void ExportData(std::string format, std::shared_ptr<PSTDFile> file, std::string output,
                                    std::vector<int> domains, int startFrame, int endFrame);
        };
    }
}


#endif //OPENPSTD_HDF5_H
