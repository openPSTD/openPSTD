//
// Created by michiel on 1-2-2016.
//

#ifndef OPENPSTD_EXPORTCLI_H
#define OPENPSTD_EXPORTCLI_H

#include <boost/program_options.hpp>
#include <shared/PSTDFile.h>

namespace OpenPSTD
{
    namespace CLI
    {
        namespace po = boost::program_options;

        class CLIExport
        {
        public:
            virtual std::vector<std::string> GetFormats() = 0;
            virtual void AddOptions(po::options_description_easy_init add_option) = 0;

            virtual void Execute(std::string format, std::shared_ptr <Shared::PSTDFile> file, std::string directory, std::string name,
                                 std::vector<int> domains, int startFrame, int endFrame, po::variables_map input) = 0;
        };

        class CLIImageExport: public CLIExport
        {
        public:
            virtual std::vector<std::string> GetFormats();

            virtual void AddOptions(po::options_description_easy_init add_option) override;

            virtual void Execute(std::string format, std::shared_ptr <Shared::PSTDFile> file, std::string directory, std::string name,
                                 std::vector<int> domains, int startFrame, int endFrame, po::variables_map input) override;
        };

        class CLIHDF5Export: public CLIExport
        {
        public:
            virtual std::vector<std::string> GetFormats();

            virtual void AddOptions(po::options_description_easy_init add_option) override;

            virtual void Execute(std::string format, std::shared_ptr <Shared::PSTDFile> file, std::string directory, std::string name,
                                 std::vector<int> domains, int startFrame, int endFrame, po::variables_map input) override;
        };
    }
}




#endif //OPENPSTD_EXPORTCLI_H
