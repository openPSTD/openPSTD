//
// Created by michiel on 1-2-2016.
//

#include "exportCLI.h"
#include <boost/program_options.hpp>
#include <shared/export/Image.h>
#include <shared/export/HDF5Export.h>


namespace po = boost::program_options;
using namespace OpenPSTD::CLI;

std::vector<std::string> CLIImageExport::GetFormats()
{
    Shared::ExportImage realExport;
    return realExport.GetFormats();
}

void CLIImageExport::AddOptions(po::options_description_easy_init add_option)
{
    add_option("image-fullview", po::value<bool>()->default_value(true), "shows all the domains in a single image per frame");
}

void CLIImageExport::Execute(std::string format, std::shared_ptr <Shared::PSTDFile> file, std::string directory, std::string name,
                          std::vector<int> domains, int startFrame, int endFrame, po::variables_map input)
{
    Shared::ExportImage realExport;
    realExport.SetFullView(input["image-fullview"].as<bool>());
    realExport.ExportData(format, file, directory, name, domains, startFrame, endFrame);
}



std::vector<std::string> CLIHDF5Export::GetFormats()
{
    std::vector<std::string> format;
    format.push_back("application/x-hdf");
    return format;
}

void CLIHDF5Export::AddOptions(po::options_description_easy_init add_option)
{

}

void CLIHDF5Export::Execute(std::string format, std::shared_ptr<Shared::PSTDFile> file, std::string directory,
                            std::string name, std::vector<int> domains, int startFrame, int endFrame,
                            po::variables_map input)
{
    OpenPSTD::Shared::HDF5 realExport;
    realExport.ExportData(format, file, directory+"/"+name+".h5", domains, startFrame, endFrame);
}