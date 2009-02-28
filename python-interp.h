#ifndef __LINGOT_INTERP__
#define __LINGOT_INTERP__

extern "C" {
	
#include <Python.h>

static PyObject *vkbd_sendkey(PyObject *self, PyObject *args);
static PyObject *vkbd_sendmouse_relative_hz(PyObject *self, PyObject *args);
static PyObject *vkbd_sendmouse_relative_vt(PyObject *self, PyObject *args);

void call_callback_function(float frequency);
void init_interp(void);
void terminate_interp(void);

static PyMethodDef VkbdMethods[] = {
    {"sendkey",  vkbd_sendkey, METH_VARARGS,
     "Report a key. The argument must be an X11 keycode. Hint: use xev(1) to get the keycode of a given key."},
    {"sendmouse_hz", vkbd_sendmouse_relative_hz, METH_VARARGS,
     "Report an horizontal mouse move relatively to the mouse's position. A negative argument means a move to the left while a positive argument means a move to the right."},
    {"sendmouse_ve", vkbd_sendmouse_relative_vt, METH_VARARGS,
     "Report an vertical mouse move relatively to the mouse's position. A negative argument means a move to the top of the screen while a positive argument means a move to the bottom."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

}



#endif
