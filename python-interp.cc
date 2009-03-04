#include "python-interp.h"

static 	PyObject *pName, *pModule, *pDict, *pFunc;
   

static PyObject *vkbd_sendkey(PyObject *self, PyObject *args)
{
	const char *key;
	char command[32];
	int sts;

	if (!PyArg_ParseTuple(args, "c", &key))
		return NULL;
	snprintf(command, 32, "xte \"key %c\"", key);
	sts = system(command);
	return Py_BuildValue("i", sts);
}

static PyObject *vkbd_sendmouse_relative_hz(PyObject *self, PyObject *args)
{
	const char *key;
	char command[32];
	int sts;
	int i;
	
	if (!PyArg_ParseTuple(args, "d", &key))
		return NULL;
	i = atoi(key);
	
	if (i < 0) 
		snprintf(command, 32, "xte \"mousermove %d 0\"", i);
	sts = system(command);
	return Py_BuildValue("i", sts);
}

static PyObject *vkbd_sendmouse_relative_vt(PyObject *self, PyObject *args)
{
	const char *key;
	char command[32];
	int sts;
	int i;
	
	if (!PyArg_ParseTuple(args, "d", &key))
		return NULL;
	i = atoi(key);
	
	if (i < 0) 
		snprintf(command, 32, "xte \"mousermove 0 %d\"", i);
	sts = system(command);
	return Py_BuildValue("i", sts);
}

void init_interp(void)
{

	int i;

	Py_Initialize();
	Py_InitModule("vkbd", VkbdMethods);

	char *path = Py_GetPath();
	int len = strlen(path) + 64;
	char *newpath = (char *) malloc(len);
	if (newpath == NULL) {
		printf("Unable to alloc memory to set the python path !\n");
		exit(EXIT_FAILURE);
	}
	snprintf(newpath, len, "%s:%s/%s/", path, getenv("HOME"), ".fretboard");

	PySys_SetPath(newpath);

	free(path);
	free(newpath);
	
	pName = PyString_FromString("fretboard_callback");

	pModule = PyImport_Import(pName);

	if (pModule == NULL) {
		perror("Unable to open fretboard_callback.py !");
		PyErr_Print();
		exit(EXIT_FAILURE);
	}

	Py_DECREF(pName);

	pFunc = PyObject_GetAttrString(pModule, "callback");

	if (!pFunc || !PyCallable_Check(pFunc)) {
		Py_DECREF(pFunc);
		Py_DECREF(pModule);
		PyErr_Print();
		exit(EXIT_FAILURE);
	}

	call_callback_function(3.4);
}

void terminate_interp(void)
{
	Py_Finalize();
}

void call_callback_function(float frequency)
{

	float freq = rint(frequency);
	if(PyObject_CallFunction(pFunc, "f", freq) == NULL)
		PyErr_Print();

} 
