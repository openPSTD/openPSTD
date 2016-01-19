//////////////////////////////////////////////////////////////////////////
// This file is part of openPSTD.                                       //
//                                                                      //
// openPSTD is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// openPSTD is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with openPSTD.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Date:
//
//
// Authors:
//
//
//////////////////////////////////////////////////////////////////////////

#include "main-cli.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "kernel/ConsoleOutput.h"
#include "kernel/PSTDKernel.h"
#include <boost/regex.hpp>
#include <PSTDFile.h>

namespace po = boost::program_options;
using namespace boost;
using namespace boost::program_options;

namespace CLI
{



    struct DomainAdd
    {
    public:
        DomainAdd()
                : x1(0), y1(0), x2(0), y2(0)
        { }
        DomainAdd(float x1, float y1, float x2, float y2)
                : x1(x1), y1(y1), x2(x2), y2(y2)
        { };
        float x1, y1, x2, y2;
    };

    struct DomainChange
    {
    public:
        DomainChange()
                : id(-1), x1(0), y1(0), x2(0), y2(0)
        { }

        DomainChange(int id, float x1, float y1, float x2, float y2)
                : id(id), x1(x1), y1(y1), x2(x2), y2(y2)
        { };
        int id;
        float x1, y1, x2, y2;
    };

    struct EdgeAbsorption
    {
    public:
        int id;
        char edge;
        float value;

        EdgeAbsorption()
                : id(-1), edge('t'), value(0)
        {  }

        EdgeAbsorption(int id, char edge, float value)
                : id(id), edge(edge), value(value)
        { }
    };

    struct EdgeLR
    {
    public:
        int id;
        char edge;
        bool value;

        EdgeLR()
                : id(-1), edge('t'), value(true)
        {  }

        EdgeLR(int id, char edge, float value)
                : id(id), edge(edge), value(value)
        { }
    };

    struct SpeakerReceiverAdd
    {
    public:
        float x, y;

        SpeakerReceiverAdd()
                : x(0), y(0)
        { }

        SpeakerReceiverAdd(float x, float y)
                : x(x), y(y)
        { }
    };
}

void validate(boost::any& v, const std::vector<std::string>& values, CLI::DomainAdd*, int)
{
    static regex r("<([^,]*),([^,]*),([^,]*),([^,]*)>");

    validators::check_first_occurrence(v);
    const std::string& s = validators::get_single_string(values);

    smatch match;
    if (regex_match(s, match, r)) {
        v = any(CLI::DomainAdd(lexical_cast<int>(match[1]), lexical_cast<float>(match[2]), lexical_cast<float>(match[3]),
                             lexical_cast<float>(match[4])));
    } else {
        throw validation_error(validation_error::invalid_option_value);
    }
}


void validate(boost::any& v, const std::vector<std::string>& values, CLI::DomainChange*, int)
{
    static regex r("\\(([^,]*),<([^,]*),([^,]*),([^,]*),([^,]*)>\\)");

    validators::check_first_occurrence(v);
    const std::string& s = validators::get_single_string(values);

    smatch match;
    if (regex_match(s, match, r)) {
        v = any(CLI::DomainChange(lexical_cast<int>(match[1]), lexical_cast<float>(match[2]), lexical_cast<float>(match[3]),
                             lexical_cast<float>(match[4]), lexical_cast<float>(match[5])));
    } else {
        throw validation_error(validation_error::invalid_option_value);
    }
}

void validate(boost::any& v, const std::vector<std::string>& values, CLI::EdgeAbsorption*, int)
{
    static regex r("\\(([^,]*),([tdlr]),([^,]*)\\)");

    validators::check_first_occurrence(v);
    const std::string& s = validators::get_single_string(values);

    smatch match;
    if (regex_match(s, match, r)) {
        char edge;
        if(match[2].compare("l")) edge = 'l';
        else if(match[2].compare("r")) edge = 'r';
        else if(match[2].compare("t")) edge = 't';
        else if(match[2].compare("b")) edge = 'b';
        v = any(CLI::EdgeAbsorption(lexical_cast<int>(match[1]), edge, lexical_cast<float>(match[3])));
    } else {
        throw validation_error(validation_error::invalid_option_value);
    }
}

void validate(boost::any& v, const std::vector<std::string>& values, CLI::EdgeLR*, int)
{
    static regex r("\\(([^,]*),([tdlr]),(true)|(false)\\)");

    validators::check_first_occurrence(v);
    const std::string& s = validators::get_single_string(values);

    smatch match;
    if (regex_match(s, match, r)) {
        char edge;
        if(match[2].compare("l")) edge = 'l';
        else if(match[2].compare("r")) edge = 'r';
        else if(match[2].compare("t")) edge = 't';
        else if(match[2].compare("b")) edge = 'b';
        v = any(CLI::EdgeLR(lexical_cast<int>(match[1]), edge, match[3].matched));
    } else {
        throw validation_error(validation_error::invalid_option_value);
    }
}

void validate(boost::any& v, const std::vector<std::string>& values, CLI::SpeakerReceiverAdd*, int)
{
    static regex r("<([^,]*),([^,]*)>");

    validators::check_first_occurrence(v);
    const std::string& s = validators::get_single_string(values);

    smatch match;
    if (regex_match(s, match, r)) {
        v = any(CLI::SpeakerReceiverAdd(lexical_cast<int>(match[1]), lexical_cast<float>(match[2])));
    } else {
        throw validation_error(validation_error::invalid_option_value);
    }
}

std::string CreateCommand::GetName()
{
    return "create";
}

std::string CreateCommand::GetDescription()
{
    return "Create an input file, see OpenPSTD-cli create -h";
}

int CreateCommand::execute(int argc, const char **argv)
{
    po::variables_map vm;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("scene-file,f", po::value<std::string>(), "The scene file that has to be used (required)")
                ;

        po::positional_options_description p;
        p.add("scene-file", 1);

        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("scene-file") == 0)
        {
            std::cerr << "scene file is required" << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }

        std::string filename = vm["scene-file"].as<std::string>();

        PSTDFile::New(filename);
        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...)
    {
        std::cerr << "Exception of unknown type!\n";
        return 1;
    }
}

std::string ListCommand::GetName()
{
    return "list";
}

std::string ListCommand::GetDescription()
{
    return "Lists the data in an input file, see OpenPSTD-cli list -h";
}

int ListCommand::execute(int argc, const char **argv)
{
    po::variables_map vm;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("scene-file,f", po::value<std::string>(), "The scene file that has to be used (required)")
                ;

        po::positional_options_description p;
        p.add("scene-file", 1);

        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("scene-file") == 0)
        {
            std::cerr << "scene file is required" << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }

        std::string filename = vm["scene-file"].as<std::string>();

        Print(filename);
        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...)
    {
        std::cerr << "Exception of unknown type!\n";
        return 1;
    }
}

void ListCommand::Print(const std::string &filename)
{
    std::unique_ptr<PSTDFile> file = PSTDFile::Open(filename);

    auto SceneConf = file->GetSceneConf();
    std::cout << "Settings: " << std::endl;
    std::cout << "  Grid spacing: " << SceneConf->Settings.GetGridSpacing() << std::endl;
    std::cout << "  Patche rror: " << SceneConf->Settings.GetPatchError() << std::endl;
    std::cout << "  Window size: " << SceneConf->Settings.GetWindowSize() << std::endl;
    std::cout << "  Render time: " << SceneConf->Settings.GetRenderTime() << std::endl;
    std::cout << "  PML cells: " << SceneConf->Settings.GetPMLCells() << std::endl;
    std::cout << "  Attenuation of PML cells: " << SceneConf->Settings.GetAttenuationOfPMLCells() << std::endl;
    std::cout << "  Density of air: " << SceneConf->Settings.GetDensityOfAir() << std::endl;
    std::cout << "  Max frequency: " << SceneConf->Settings.GetMaxFrequency() << std::endl;
    std::cout << "  Sound speed: " << SceneConf->Settings.GetSoundSpeed() << std::endl;
    std::cout << "  FactRK: " << SceneConf->Settings.GetFactRK() << std::endl;
    std::cout << "  Save Nth: " << SceneConf->Settings.GetSaveNth() << std::endl;
    std::cout << "  Bandwidth: " << SceneConf->Settings.GetBandWidth() << std::endl;
    std::cout << "  Spectral interpolation: " << SceneConf->Settings.GetSpectralInterpolation() << std::endl;
    std::cout << "  Wave length: " << SceneConf->Settings.GetWaveLength() << std::endl;
    std::cout << "  Time step: " << SceneConf->Settings.GetTimeStep() << std::endl;
    std::cout << "  RKCoefficients: ";
    auto coef = SceneConf->Settings.GetRKCoefficients();
    for (int i = 0; i < coef.size(); ++i)
    {
        std::cout << coef[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "  Window: " << SceneConf->Settings.GetWindow() << std::endl;

    std::cout << std::endl;
    std::cout << "Speakers: " << std::endl;
    for (int i = 0; i < SceneConf->Speakers.size(); ++i)
    {
        std::cout << "  " << i << ": <" << SceneConf->Speakers[i].x() << ", " << SceneConf->Speakers[i].y() << ">" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Receivers: " << std::endl;
    for (int i = 0; i < SceneConf->Receivers.size(); ++i)
    {
        std::cout << "  " << i << ": <" << SceneConf->Receivers[i].x() << ", " << SceneConf->Receivers[i].y() << ">" << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Domains: " << std::endl;
    for (int i = 0; i < SceneConf->Domains.size(); ++i)
    {
        std::cout << "Domain " << i << ":" << std::endl;
        std::cout << "  " << "Pos: <" << SceneConf->Domains[i].TopLeft.x() << ", " << SceneConf->Domains[i].TopLeft.y() << ">" << std::endl;
        std::cout << "  " << "Size: <" << SceneConf->Domains[i].Size.x() << ", " << SceneConf->Domains[i].Size.y() << ">" << std::endl;
        std::cout << "  " << "Top: Absorption " << SceneConf->Domains[i].T.Absorption << ", LR " << SceneConf->Domains[i].T.LR << std::endl;
    }
}

std::string EditCommand::GetName()
{
    return "edit";
}

std::string EditCommand::GetDescription()
{
    return "Edits an input file, see OpenPSTD-cli edit -h";
}

int EditCommand::execute(int argc, const char **argv)
{
    /*po::variables_map vm;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("domain-add,d", po::value<std::vector<CLI::DomainAdd>>(), "Add a domain, --domain-add <x,y,size-x,size-y>, "
                        "eg --domain-add <0,0,10,10>")
                ("domain-remove,D", po::value<std::vector<int>>(), "Remove a domain, --domain-remove id")
                ("domain-change,c", po::value<std::vector<CLI::DomainChange>>(), "Change position of a domain, "
                        "--domain-change (id,<x,y,size-x,size-y>), eg --domain-change (1,<0,0,10,10>)")
                ("scene-file,f", po::value<std::string>(), "The scene file that has to be used (required)")
                ;

        po::positional_options_description p;
        p.add("scene-file", 1);

        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("scene-file") == 0)
        {
            std::cerr << "scene file is required" << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }

        std::string filename = vm["scene-file"].as<std::string>();

        //todo fix edit commands

        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...)
    {
        std::cerr << "Exception of unknown type!\n";
        return 1;
    }*/
    //todo fix this command
    return 0;
}

std::string RunCommand::GetName()
{
    return "run";
}

std::string RunCommand::GetDescription()
{
    return "Run the OpenPSTD algorithm, see OpenPSTD-cli run -h";
}

int RunCommand::execute(int argc, const char **argv)
{
    po::variables_map vm;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("scene-file,f", po::value<std::string>(), "The scene file that has to be used (required)")
                ("multithreaded,m", "use the multi-threaded solver (mutually exclusive with gpu accelerated)")
                ("gpu-accelerated,g", "Use the gpu for the calculations (mutually exclusive with multithreaded)")
                ("write-plot,p", "Plots are written to the output directory")
                ("write-array,a", "Arrays are written to the output directory")
                ;

        po::positional_options_description p;
        p.add("scene-file", 1);

        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }

        if (vm.count("scene-file") == 0)
        {
            std::cerr << "scene file is required" << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }

        if(vm.count("multithreaded")>0 && vm.count("gpu-accelerated")>0)
        {
            std::cerr << "multithreaded and gpu accelerated options are mutually exclusive" << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }

        if(vm.count("write-plot")==0 && vm.count("write-array")==0)
        {
            std::cout << "Warning: there is no output written" << std::endl;
        }

        std::string filename = vm["scene-file"].as<std::string>();

        //todo implement plot and array
        //todo implement mt and gpu

        //open file
        std::unique_ptr<PSTDFile> file = PSTDFile::Open(filename);
        //get conf for the kernel
        std::shared_ptr<PSTDFileConfiguration> conf = file->GetSceneConf();
        //create kernel
        std::unique_ptr<PSTDKernel> kernel = std::unique_ptr<PSTDKernel>(new PSTDKernel(conf));
        //create output
        std::shared_ptr<ConsoleOutput> output(new ConsoleOutput());
        //run kernel
        kernel->run(output.get());
        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...)
    {
        std::cerr << "Exception of unknown type!\n";
        return 1;
    }
}

int main(int argc, const char *argv[])
{
    std::vector<std::unique_ptr<Command>> commands;
    commands.push_back(std::unique_ptr<CreateCommand>(new CreateCommand()));
    commands.push_back(std::unique_ptr<ListCommand>(new ListCommand()));
    commands.push_back(std::unique_ptr<EditCommand>(new EditCommand()));
    commands.push_back(std::unique_ptr<RunCommand>(new RunCommand()));

    std::string command = argv[1];
    for (int i = 0; i < commands.size(); ++i)
    {
        if(command.compare(commands[i]->GetName()) == 0)
        {
            return commands[i]->execute(argc-1, argv+1);
            //one less argument, specificaly the first, that is the reason of the +1
        }
    }

    std::cout << "Usage: OpenPSTD-cli <command>" << std::endl;
    std::cout << std::endl;
    std::cout << "possible commands are: " << std::endl;
    for (int i = 0; i < commands.size(); ++i)
    {
        std::cout << "\t" << commands[i]->GetName() << "\t\t" << commands[i]->GetDescription() << std::endl;
    }



    /*po::variables_map vm;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("scene-file,f", po::value<std::string>(), "The scene file that has to be used (required)")
                ("multithreaded,m", "use the multi-threaded solver (mutually exclusive with gpu accelerated)")
                ("gpu-accelerated,g", "Use the gpu for the calculations (mutually exclusive with multithreaded)")
                ("write-plot,p", "Plots are written to the output directory")
                ("write-array,a", "Arrays are written to the output directory")
                ;

        po::positional_options_description p;
        p.add("scene-file", -1);

        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }

        if (vm.count("scene-file") == 0)
        {
            std::cerr << "scene file is required" << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }

        if(vm.count("multithreaded")>0 && vm.count("gpu-accelerated")>0)
        {
            std::cerr << "multithreaded and gpu accelerated options are mutually exclusive" << std::endl;
            std::cout << desc << std::endl;
            return 1;
        }

        if(vm.count("write-plot")==0 && vm.count("write-array")==0)
        {
            std::cout << "Warning: there is no output written" << std::endl;
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...)
    {
        std::cerr << "Exception of unknown type!\n";
        return 1;
    }

    KernelCallback *output = new ConsoleOutput();
    PSTDFileConfiguration configuration = PSTDFileConfiguration();
    std::unique_ptr<PSTDKernel> kernel = std::unique_ptr<PSTDKernel>(new PSTDKernel(std::make_shared<PSTDFileConfiguration>(configuration)));
    //std::shared_ptr<PSTDFileConfiguration> ptr = std::make_shared<PSTDFileConfiguration>(configuration);
    //kernel->run(output);

    //kernel->Run(vm["scene-file"].as<std::string>().c_str(), output);

    delete output;
    //delete kernel;

    std::cin.get();

    return 0;*/
}