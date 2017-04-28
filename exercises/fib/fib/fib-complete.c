#include <Python.h>

PyDoc_STRVAR(fib_doc, "compute the nth Fibonacci number");

static PyObject*
pyfib(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static char* keywords[] = {"n", "a", "b", NULL};
    PyObject* n_ob;
    unsigned long n;
    PyObject* a = NULL;
    PyObject* b = NULL;
    PyObject* c;

    if (!PyArg_ParseTupleAndKeywords(args,
                                     kwargs,
                                     "O|$OO:fib",
                                     keywords,
                                     &n_ob,
                                     &a,
                                     &b)) {
        return NULL;
    }

    n = PyLong_AsUnsignedLong(n_ob);
    if (PyErr_Occurred()) {
        return NULL;
    }

    if (!a) {
        if (!(a = PyLong_FromUnsignedLong(1))) {
            return NULL;
        }
    }
    else {
        Py_INCREF(a);
    }

    if (n == 0) {
        return a;
    }

    if (!b) {
        if (!(b = PyLong_FromUnsignedLong(1))) {
            Py_DECREF(a);
            return NULL;
        }
    }
    else {
        Py_INCREF(b);
    }

    while (--n > 1) {
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
    {"fib", (PyCFunction) pyfib, METH_VARARGS | METH_KEYWORDS, fib_doc},
    {NULL},
};

PyDoc_STRVAR(fib_module_doc, "provides a Fibonacci function");

PyModuleDef fib_module = {
    PyModuleDef_HEAD_INIT,
    "fib.fib",
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
