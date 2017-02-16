/**************************************************************************
 
 The MIT License (MIT)
 
 Copyright (c) 2015 Dmitry Sovetov
 
 https://github.com/dmsovetov
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 **************************************************************************/

#include <Python.h>

static PyObject* DreemchestError;

static PyObject* dreemchest_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;
    
    if (!PyArg_ParseTuple(args, "s", &command))
    {
        return NULL;
    }
    
    sts = system(command);
    
    if (sts < 0)
    {
        PyErr_SetString(DreemchestError, "System command failed");
        return NULL;
    }
    
    return Py_BuildValue("i", sts);
}

static PyMethodDef Methods[] =
{
    { "system", dreemchest_system, METH_VARARGS, "Execute a shell command." },
    { NULL,     NULL,              0,            NULL                       }   // Sentinel
};

PyMODINIT_FUNC initdreemchest(void)
{
    PyObject* module = Py_InitModule("dreemchest", Methods);
    
    if (module == NULL)
    {
        return;
    }
    
    DreemchestError = PyErr_NewException("dreemchest.error", NULL, NULL);
    Py_INCREF(DreemchestError);
    PyModule_AddObject(module, "error", DreemchestError);
}
