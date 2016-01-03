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

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "kernel/ConsoleOutput.h"
#include "kernel/PSTDKernel.h"


namespace po = boost::program_options;

int main(int argc, const char *argv[])
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

    return 0;
}