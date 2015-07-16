#include <iostream>
#include <Python.h>
#include <string>
#include <fstream>
#include <streambuf>
#include "kernel/ConsoleOutput.h"

using namespace std;

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

int main(int argc, char *argv[])
{
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

    PyObject* sceneFile = PyUnicode_FromString("emptyFile.jps");
    PyObject* multi_threaded = PyBool_FromLong(false);
    PyObject* gpu_accelerated = PyBool_FromLong(false);
    PyObject* write_plot = PyBool_FromLong(false);
    PyObject* write_array = PyBool_FromLong(false);
    PyDict_SetItemString(local_dict, "scene_file", sceneFile);
    PyDict_SetItemString(local_dict, "multi_threaded", multi_threaded);
    PyDict_SetItemString(local_dict, "gpu_accelerated", gpu_accelerated);
    PyDict_SetItemString(local_dict, "write_plot", write_plot);
    PyDict_SetItemString(local_dict, "write_array", write_array);

    Py_DECREF(sceneFile);
    Py_DECREF(multi_threaded);
    Py_DECREF(gpu_accelerated);
    Py_DECREF(write_plot);
    Py_DECREF(write_array);

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