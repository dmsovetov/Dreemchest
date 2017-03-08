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
#include <Dreemchest.h>

static PyObject* DreemchestError;

// ** export_mesh
static PyObject* export_mesh(PyObject *self, PyObject *args)
{
    const char* input;
	const char* output;

    if (!PyArg_ParseTuple(args, "ss", &input, &output))
    {
		PyErr_SetString(DreemchestError, "failed to parse arguments");
        return NULL;
    }
    
	printf("export_mesh: %s -> %s\n", input, output);
	return Py_None;
}

// ** export_texture
static PyObject* export_texture(PyObject *self, PyObject *args)
{
	const char* input;
	const char* output;
	double      scaleFactor = 1.0;

	if (!PyArg_ParseTuple(args, "ss|d", &input, &output, &scaleFactor))
	{
		PyErr_SetString(DreemchestError, "failed to parse arguments");
		return NULL;
	}

	printf("export_texture: %s -> %s [%2.2f]\n", input, output, scaleFactor);
	return Py_None;
}

// ** compress_texture
static PyObject* compress_texture(PyObject *self, PyObject *args)
{
	const char* input;
	const char* output;
	const char* format;

	if (!PyArg_ParseTuple(args, "sss", &input, &output, &format))
	{
		PyErr_SetString(DreemchestError, "failed to parse arguments");
		return NULL;
	}

	printf("compress_texture: %s -> %s [%s]\n", input, output, format);
	return Py_None;
}

// ** export_atlas
static PyObject* export_atlas(PyObject *self, PyObject *args)
{
	PyObject*   input;
	const char* output;

	if (!PyArg_ParseTuple(args, "Os", &input, &output))
	{
		PyErr_SetString(DreemchestError, "failed to parse arguments");
		return NULL;
	}

	PyObject* iter = PyObject_GetIter(input);
	if (!iter)
	{
		PyErr_SetString(DreemchestError, "input is not iterable");
		return NULL;
	}

	printf("export_atlas: %s <--\n", output);

	while (true)
	{
		PyObject *next = PyIter_Next(iter);

		if (!next)
		{
			break;
		}

		if (!PyString_Check(next))
		{
			printf("string expected\n");
			continue;
		}

		const char* fileName = PyString_AsString(next);
		printf("\t%s\n", fileName);
	}

	return Py_None;
}

// ** cubemap_from_cross
static PyObject* cubemap_from_cross(PyObject *self, PyObject *args)
{
	const char* input;
	const char* output;

	if (!PyArg_ParseTuple(args, "ss", &input, &output))
	{
		PyErr_SetString(DreemchestError, "failed to parse arguments");
		return NULL;
	}

	printf("cubemap_from_cross: %s -> %s\n", input, output);
	return Py_None;
}

// ** cubemap_from_panorama
static PyObject* cubemap_from_panorama(PyObject *self, PyObject *args)
{
	const char* input;
	const char* output;

	if (!PyArg_ParseTuple(args, "ss", &input, &output))
	{
		PyErr_SetString(DreemchestError, "failed to parse arguments");
		return NULL;
	}

	printf("cubemap_from_panorama: %s -> %s\n", input, output);
	return Py_None;
}

// ** cubemap_convolve
static PyObject* cubemap_convolve(PyObject *self, PyObject *args)
{
	const char* input;
	const char* output;
	int			samples;
	double		power;

	if (!PyArg_ParseTuple(args, "ssid", &input, &output, &samples, &power))
	{
		PyErr_SetString(DreemchestError, "failed to parse arguments");
		return NULL;
	}

	printf("cubemap_convolve: %s -> %s [samples=%d, power=%2.2f]\n", input, output, samples, power);
	return Py_None;
}

// ** shader_convert
static PyObject* shader_convert(PyObject *self, PyObject *args)
{
	const char* input;
	const char* output;
	const char* generator;

	if (!PyArg_ParseTuple(args, "sss", &input, &output, &generator))
	{
		PyErr_SetString(DreemchestError, "failed to parse arguments");
		return NULL;
	}

	printf("shader_convert: %s -> %s [%s]\n", input, output, generator);
	return Py_None;
}

// ** methods
static PyMethodDef methods[] =
{
    { "export_mesh",			export_mesh,			METH_VARARGS, "Converts an input mesh to an internal Dreemchest mesh format." },
	{ "export_texture",			export_texture,			METH_VARARGS, "" },
	{ "compress_texture",		compress_texture,		METH_VARARGS, "" },
	{ "export_atlas",			export_atlas,			METH_VARARGS, "" },
	{ "cubemap_from_cross",		cubemap_from_cross,		METH_VARARGS, "" },
	{ "cubemap_from_panorama",	cubemap_from_panorama,	METH_VARARGS, "" },
	{ "cubemap_convolve",		cubemap_convolve,		METH_VARARGS, "" },
	{ "shader_convert",			shader_convert,			METH_VARARGS, "" },
    { NULL,						NULL,					0,            NULL	}   // Sentinel
};

// ** initdreemchest
PyMODINIT_FUNC initdreemchest(void)
{
    PyObject* module = Py_InitModule("dreemchest", methods);
    
    if (module == NULL)
    {
        return;
    }
    
    DreemchestError = PyErr_NewException("dreemchest.error", NULL, NULL);
    Py_INCREF(DreemchestError);
    PyModule_AddObject(module, "error", DreemchestError);
}
