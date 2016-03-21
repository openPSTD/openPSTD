//
// Created by michiel on 24-2-2016.
//

#include "HDF5Export.h"
#include "kernel/MockKernel.h"

#include <hdf5.h>
#include <hdf5_hl.h>

#include <boost/lexical_cast.hpp>

namespace OpenPSTD
{
    namespace Shared
    {
        OPENPSTD_SHARED_EXPORT void HDF5::ExportData(std::string format, std::shared_ptr <PSTDFile> file, std::string output,
                              std::vector<int> domains, int startFrame, int endFrame)
        {
            hid_t file_id;
            Kernel::MockKernel k;

            file_id = H5Fcreate(output.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

            H5Gcreate2(file_id, "/frame", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

            k.initialize_kernel(file->GetSceneConf());
            auto metadata = k.get_metadata();

            if (domains.size() == 0)
            {
                int domainCount = file->GetResultsDomainCount();

                for (int d = 0; d < domainCount; ++d)
                {
                    domains.push_back(d);
                }
            }

            for(int d : domains)
            {
                std::string domainLoc = "/frame/" + boost::lexical_cast<std::string>(d);
                H5Gcreate2(file_id, domainLoc.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

                if (startFrame == -1) startFrame = 0;
                if (endFrame == -1) endFrame = file->GetResultsFrameCount(d) - 1;
                for (int f = startFrame; f <= endFrame; ++f)
                {
                    std::string location = domainLoc + "/" + boost::lexical_cast<std::string>(f);

                    std::vector<hsize_t> size;
                    size.push_back(metadata.DomainMetadata[d][0]);
                    size.push_back(metadata.DomainMetadata[d][1]);

                    //get data and write to file
                    auto data = file->GetResultsFrame(f, d);
                    H5LTmake_dataset(file_id, location.c_str(), 2, size.data(), H5T_NATIVE_FLOAT, data->data());
                }
            }

            /* close file */
            H5Fclose (file_id);
        }
    }
}