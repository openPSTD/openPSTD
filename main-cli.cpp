#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "kernel/ConsoleOutput.h"
#include "kernel/KernelFacade.h"


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

    KernelFacadeCallback* output = new ConsoleOutput();
    KernelFacade* kernel = new KernelFacade();

    KernelConfiguration configuration;
    configuration.multiThreaded = vm.count("multithreaded")>0;
    configuration.gpuAccelerated = vm.count("gpu-accelerated")>0;
    configuration.writePlot = vm.count("write-plot")>0;
    configuration.writeArray = vm.count("write-array")>0;
    kernel->Configure(configuration);

    kernel->Run(vm["scene-file"].as<std::string>().c_str(), output);

    delete output;
    delete kernel;

    std::cin.get();

    return 0;
}