//
// Created by michiel on 12-7-2015.
//

#include "ConsoleOutput.h"
#include <iostream>

static PyObject* ConsoleOutput_func(PyObject *self, PyObject *args, PyObject *keywds)
{
    const char* status;
    const char* message;
    PyObject* scene;
    PyObject* pstd_desc;
    PyObject* exception;
    int frame;

    static char *kwlist[] = {"status", "message", "frame", "scene", "pstd_desc", "exception", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "ss|iOOO", kwlist,
                                     &status, &message, &frame, &scene, &pstd_desc, &exception))
        return NULL;

    if(strcmp(status, "error") == 0)
    {
        std::cout << "Error encountered while running OpenPSTD:" << std::endl;
        std::cout << message << std::endl;
    }
    else if(strcmp(status, "starting") == 0)
    {
        std::cout << message << std::endl;
    }
    else if(strcmp(status, "running") == 0)
    {
        std::cout << message << std::endl;
    }
    else if(strcmp(status, "success") == 0)
    {
        std::cout << message << std::endl;
    }

    Py_RETURN_NONE;
}

static PyMethodDef ConsoleOutputMethods[] = {
        {"func", (PyCFunction)ConsoleOutput_func, METH_VARARGS | METH_KEYWORDS, "function that processes the output of the PSTD class."},
        {NULL, NULL, 0, NULL}
};

static PyModuleDef ConsoleOutputModule = {
        PyModuleDef_HEAD_INIT,
        "ConsoleOutput",
        NULL,
        -1,
        ConsoleOutputMethods,
        NULL,
        NULL,
        NULL,
        NULL
};

PyMODINIT_FUNC PyInit_ConsoleOutput(void)
{
    return PyModule_Create(&ConsoleOutputModule);
}