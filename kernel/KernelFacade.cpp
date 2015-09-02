//
// Created by michiel on 18-7-2015.
//

#include "KernelFacade.h"

#include <Python.h>
#include <iostream>

//-----------------------------------------------------------------------------
// callback extension
KernelFacadeCallback* callbackObject;

static PyObject* KernelCallback_func(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char* status;
    const char* message;
    PyObject* scene;
    PyObject* pstd_desc;
    PyObject* exception;
    int frame;

    static const char* kwlist[] = {"status", "message", "frame", "scene", "pstd_desc", "exception", NULL};

    static char* kwlist2[] = {strdup(kwlist[0]), strdup(kwlist[1]), strdup(kwlist[2]), strdup(kwlist[3]),
                              strdup(kwlist[4]), strdup(kwlist[5]), nullptr};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "ss|iOOO", kwlist2,
                                     &status, &message, &frame, &scene, &pstd_desc, &exception))
        return NULL;

    if(strcmp(status, "error") == 0)
    {
        callbackObject->Callback(CALLBACKSTATUS_ERROR, message, -1);
    }
    else if(strcmp(status, "starting") == 0)
    {
        callbackObject->Callback(CALLBACKSTATUS_STARTING, message, -1);
    }
    else if(strcmp(status, "running") == 0)
    {
        callbackObject->Callback(CALLBACKSTATUS_RUNNING, message, frame);
    }
    else if(strcmp(status, "success") == 0)
    {
        callbackObject->Callback(CALLBACKSTATUS_SUCCES, message, -1);
    }

    Py_RETURN_NONE;
}

static PyMethodDef KernelCallbackMethods[] = {
        {"func", (PyCFunction)KernelCallback_func, METH_VARARGS | METH_KEYWORDS, "function that processes the output of the PSTD class."},
        {NULL, NULL, 0, NULL}
};

static PyModuleDef KernelCallbackModule = {
        PyModuleDef_HEAD_INIT,
        "KernelCallback",
        NULL,
        -1,
        KernelCallbackMethods,
        NULL,
        NULL,
        NULL,
        NULL
};

PyMODINIT_FUNC PyInit_KernelCallback(void)
{
    return PyModule_Create(&KernelCallbackModule);
}

//-----------------------------------------------------------------------------
// C++ interface of the kernel

void KernelFacade::OutputPythonHomeAndPath()
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

void KernelFacade::Configure(KernelConfiguration configuration)
{
    this->configuration = configuration;
}

void KernelFacade::Run(std::string sceneFile, KernelFacadeCallback* callback)
{
    callbackObject = callback;

    std::string pythonCode = "import json\n"
            "import os, sys\n"
            "sys.path.insert(0,'')\n"
            "from kernel.pstd import PSTD\n"
            "import KernelCallback\n"
            "f = open(scene_file, 'r')\n"
            "scene_desc = json.load(f)\n"
            "f.close()\n"
            "def output_to_console(arg):\n"
            "   import KernelCallback\n"
            "   KernelCallback.func(**arg)\n"
            "pstd = PSTD(multi_threaded, gpu_accelerated, write_plot, write_array, scene_desc, output_to_console)\n"
            "pstd.run()";

    //Py_SetProgramName("");  /* optional but recommended */

    OutputPythonHomeAndPath();

    PyImport_AppendInittab("KernelCallback", &PyInit_KernelCallback);

    Py_Initialize();

    PyObject* main_module = PyImport_AddModule("__main__");
    PyObject* global_dict = PyModule_GetDict(main_module);
    PyObject* local_dict = PyDict_New();

    PyObject* pySceneFile = PyUnicode_FromString(sceneFile.c_str());
    PyObject* pyMulti_threaded = PyBool_FromLong(this->configuration.multiThreaded);
    PyObject* pyGpu_accelerated = PyBool_FromLong(this->configuration.gpuAccelerated);
    PyObject* pyWrite_plot = PyBool_FromLong(this->configuration.writePlot);
    PyObject* pyWrite_array = PyBool_FromLong(this->configuration.writeArray);
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

    auto result = PyRun_String(pythonCode.c_str(), Py_file_input, global_dict, local_dict);
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
    Py_Finalize();
}
