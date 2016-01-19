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


struct DomainAdd
{
public:
    DomainAdd(float x1, float y1, float x2, float y2)
            : x1(x1), y1(y1), x2(x2), y2(y2) {};
    float x1, y1, x2, y2;
};

struct DomainChange
{
public:
    DomainChange(int id, float x1, float y1, float x2, float y2)
            : id(id), x1(x1), y1(y1), x2(x2), y2(y2) {};
    int id;
    float x1, y1, x2, y2;
};

namespace po = boost::program_options;


void validate(boost::any& v,
              const std::vector<std::string>& values,
              DomainChange*, int)
{
    using namespace boost;
    using namespace boost::program_options;

    //static regex r("<([^,]*),([^,]*),([^,]*),([^,]*)>");
    static regex r("\\(([^,]*),<([^,]*),([^,]*),([^,]*),([^,]*)>\\)");

    // Make sure no previous assignment to 'a' was made.
    validators::check_first_occurrence(v);
    // Extract the first string from 'values'. If there is more than
    // one string, it's an error, and exception will be thrown.
    const std::string& s = validators::get_single_string(values);

    // Do regex match and convert the interesting part to
    // float.
    smatch match;
    if (regex_match(s, match, r)) {
        v = any(DomainChange(lexical_cast<int>(match[1]), lexical_cast<float>(match[2]), lexical_cast<float>(match[3]),
                          lexical_cast<float>(match[4]), lexical_cast<float>(match[5])));
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
    return 0;
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
    return 0;
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