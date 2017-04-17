#include <Python.h>

static unsigned long
fib(unsigned long n)
{
    unsigned long a = 1;
    unsigned long b = 1;
    unsigned long c;

    if (n <= 1) {
        return 1;
    }

    while (--n > 0) {
        c = a + b;
        a = b;
        b = c;
    }

    return b;
}

PyDoc_STRVAR(fib_doc, "compute the nth Fibonacci number");

static PyObject*
fib(PyObject* self, PyObject* n)
{
    unsigned long as_unsigned_long = PyLong_AsUnsignedLong(n);
    return PyLong_FromUnsignedLong(cfib(as_unsigned_long));
}

PyMethodDef methods[] = {
    {"fib", (PyCFunction) fib, METH_O, fib_doc},
    {NULL},
};

PyDoc_STRVAR(fib_module_doc, "provides an nth Fibonacci number function");

PyModuleDef fib_module = {
    PyModuleDef_HEAD_INIT,
    "fib",
    fib_doc,
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