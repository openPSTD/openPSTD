#include <boost/program_options.hpp>
#include <iostream>
#include <Python.h>
#include <string>
#include <fstream>
#include "kernel/ConsoleOutput.h"


namespace po = boost::program_options;

void OutputPythonHomeAndPath()
{
    wchar_t * home = Py_GetPythonHome();
    wchar_t * path = Py_GetPath();

    if(home != nullptr)
    {
        std::wcout << "Python home: " << home << std::endl;
    }
    else
    {
        std::wcout << "Python home: null" << std::endl;
    }
    if(path != nullptr)
    {
        std::wcout << "Python path: " << path << std::endl;
    }
    else
    {
        std::wcout << "Python path: null" << std::endl;
    }

}

int main(int argc, const char *argv[])
{
    po::variables_map vm;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,H", "produce help message")
                ("scene-file,F", po::value<std::string>(), "The scene file that has to be used (required)")
                ("multithreaded,M", "use the multi-threaded solver (mutually exclusive with gpu accelerated)")
                ("gpu-accelerated,G", "Use the gpu for the calculations (mutually exclusive with multithreaded)")
                ("write-plot,P", "Plots are written to the output directory")
                ("write-array,A", "Arrays are written to the output directory")
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

    OutputPythonHomeAndPath();

    std::string str;

    str = "import json\n"
            "import os, sys\n"
            "from kernel.pstd import PSTD\n"
            "import ConsoleOutput\n"
            "f = open(scene_file, 'r')\n"
            "scene_desc = json.load(f)\n"
            "f.close()\n"
            "def output_to_console(arg):\n"
            "   import ConsoleOutput\n"
            "   ConsoleOutput.func(**arg)\n"
            "pstd = PSTD(multi_threaded, gpu_accelerated, write_plot, write_array, scene_desc, output_to_console)\n"
            "pstd.run()";

    //Py_SetProgramName("");  /* optional but recommended */
    std::cout << "py console output append" << std::endl;
    PyImport_AppendInittab("ConsoleOutput", &PyInit_ConsoleOutput);
    std::cout << "py init" << std::endl;
    Py_Initialize();

    OutputPythonHomeAndPath();

    std::cout << "py run" << std::endl;

    PyObject* main_module = PyImport_AddModule("__main__");
    PyObject* global_dict = PyModule_GetDict(main_module);
    PyObject* local_dict = PyDict_New();

    PyObject* pySceneFile = PyUnicode_FromString(vm["scene-file"].as<std::string>().c_str());
    PyObject* pyMulti_threaded = PyBool_FromLong(vm.count("multithreaded")>0);
    PyObject* pyGpu_accelerated = PyBool_FromLong(vm.count("gpu-accelerated")>0);
    PyObject* pyWrite_plot = PyBool_FromLong(vm.count("write-plot")>0);
    PyObject* pyWrite_array = PyBool_FromLong(vm.count("write-array")>0);
    PyDict_SetItemString(local_dict, "scene_file", pySceneFile);
    PyDict_SetItemString(local_dict, "multi_threaded", pyMulti_threaded);
    PyDict_SetItemString(local_dict, "gpu_accelerated", pyGpu_accelerated);
    PyDict_SetItemString(local_dict, "write_plot", pyWrite_plot);
    PyDict_SetItemString(local_dict, "write_array", pyWrite_array);

    Py_DECREF(pySceneFile);
    Py_DECREF(pyMulti_threaded);
    Py_DECREF(pyGpu_accelerated);
    Py_DECREF(pyWrite_plot);
    Py_DECREF(pyWrite_array);

    auto result = PyRun_String(str.c_str(), Py_file_input, global_dict, local_dict);
    //Check for system exit exception
    if(PyErr_Occurred())
    {
        if(PyErr_ExceptionMatches(PyExc_SystemExit))
        {
            //handle system exit
            PyErr_Clear();
        }
        else
        {
            PyErr_Print();
        }
    }
    std::cout << "py finalize" << std::endl;
    Py_Finalize();
    std::cout << "end" << std::endl;
    return 0;
}