#include <Python.h>

PyDoc_STRVAR(fib_doc, "compute the nth Fibonacci number");

static PyObject*
pyfib(PyObject* self, PyObject* n)
{
    PyObject* a = NULL;
    PyObject* b = NULL;
    PyObject* c;

    unsigned long n_as_unsigned_long = PyLong_AsUnsignedLong(n);
    if (PyErr_Occurred()) {
        return NULL;
    }

    if (!(a = PyLong_FromUnsignedLong(1))) {
        return NULL;
    }

    if (n_as_unsigned_long == 0) {
        return a;
    }

    if (!(b = PyLong_FromUnsignedLong(1))) {
        Py_DECREF(a);
        return NULL;
    }

    while (--n_as_unsigned_long > 1) {
        c = PyNumber_Add(a, b);
        Py_DECREF(a);

        if (!c) {
            Py_DECREF(b);
            return NULL;
        }

        a = b;
        b = c;
    }
    Py_DECREF(a);
    return b;
}

PyMethodDef methods[] = {
    {"fib", (PyCFunction) pyfib, METH_O, fib_doc},
    {NULL},
};

PyDoc_STRVAR(fib_module_doc, "provides a Fibonacci function");

PyModuleDef fib_module = {
    PyModuleDef_HEAD_INIT,
    "fib",
    fib_module_doc,
    -1,
    methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC
PyInit_fib(void)
{
    return PyModule_Create(&fib_module);
}
