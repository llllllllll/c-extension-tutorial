Writing a New Function in C
===========================

Let's begin with a simple C function to compute Fibonacci numbers:

.. code-block:: c

   static unsigned long
   cfib(unsigned long n)
   {
       unsigned long a = 1;
       unsigned long b = 1;
       unsigned long c;

       if (n <= 1) {
           return 1;
       }

       while (--n > 1) {
           c = a + b;
           a = b;
           b = c;
       }

       return b;
   }

Now that we have a a C function we need to provide a way to convert our
:c:type:`PyObject*`\s to and from C :c:type:`unsigned long` objects to invoke
this from Python.

.. note::

   It is important to declare all functions as ``static``. This is normally good
   practice in C but it is especially important when linking with CPython which
   is a very large project. We don't want to pollute the namespace.

:c:type:`PyLongObject*`
-----------------------

Python :c:type:`int` objects are concretely typed as a :c:type:`PyLongObject`.

.. note::

   Python :c:type:`int` objects are called :c:type:`PyLongObject` in the C API
   as a holdover from when :class:`int` and :class:`long` were different in
   Python 2. In Python 3: :class:`int` is arbitrarily sized like the old Python
   2 :class:`long`.

Adapting Long Objects
---------------------

For many primitive C types, CPython provides functions to convert to and from
Python objects. For :c:type:`unsigned long`, we can use
:c:func:`PyLong_FromUnsignedLong` and :c:func:`PyLong_AsUnsignedLong` to convert
to and from C unsigned long objects.

We can then write a wrapping function to do this conversion for us:

.. code-block:: c

   static PyObject*
   pyfib(PyObject* self, PyObject* n)
   {
       unsigned long as_unsigned_long = PyLong_AsUnsignedLong(n);
       PyObject *result = PyLong_FromUnsignedLong(cfib(as_unsigned));
       return result;
   }

The wrapping function needs a ``PyObject* self`` argument. This is a requirement
for all functions and methods in the C API. The second argument ``PyObject* n``
is the input we want to receive from the Python caller.

.. note::

   Right now we are ignoring the fact that ``n`` might not actually be a
   :c:type:`PyLongObject*`. We will get to error handling later.

Creating a Python Callable Object
---------------------------------

Given our wrapping function, we still need a way to pass the function to Python
to be called. To do this, we need to associate some extra metadata with our C
function. This metadata is stored along with the function in a
:c:type:`PyMethodDef` structure.

This structure defines the name of the function as it will appear in Python, the
pointer to the C implementation, information about how to invoke the function,
and finally the docstring.

A :c:type:`PyMethodDef` for our ``pyfib`` function looks like:

.. code-block:: c

   PyDOC_STRVAR(fib_doc, "computes the nth Fibonacci number);
   PyMethodDef fib_method = {
       "fib",                /* The name as a C string. */
       (PyCFunction) pyfib,  /* The C function to invoke. */
       METH_O,               /* Flags telling Python how to invoke ``pyfib`` */
       fib_doc,              /* The docstring as a C string. */
   };


:c:func:`PyDoc_STRVAR`
~~~~~~~~~~~~~~~~~~~~~~

We don't just use a normal :c:type:`const char*` for the docstring because
CPython can be compiled to not include docstrings. This is useful on platforms
with less available RAM. To properly respect this compile time option we wrap
all docstrings in the :c:func:`PyDoc_STRVAR` macro.

:c:macro:`METH_O`
~~~~~~~~~~~~~~~~~

For our function we only accept a single argument as a :c:type:`PyObject*` so we
can use the :c:macro:`METH_O` flag. For a list of the available flags see:
:c:member:`PyMethodDef.ml_flags`.

Creating a Python Module
~~~~~~~~~~~~~~~~~~~~~~~~

The last thing we need to do to export our ``fib`` function to Python is put it
in a module to be imported. Like a :c:type:`PyMethodDef`, a
:c:type:`PyModuleDef` is some metadata which describes a Python module object.

.. code-block:: c

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

The module initialization always starts with ``PyModuleDef_HEAD_INIT`` to setup
the part of the ``PyModuleDef`` which is managed by CPython.

Next is the name of the module as a C string.

After the name is the module's docstring. Like in a :c:type:`PyMethodDef`, we
need to use :c:func:`PyDoc_STRVAR` to define the docstring so that it can be
disabled at compile time.

The ``-1`` is the size of the module's global state. For our simple ``fib``
module we don't have any state so this can be set to ``-1``.

Next is a :c:data:`NULL` terminated array of methods to put at module scope in
this module. We have created an array with just our ``pyfib`` function, but we
could include more than one function if we needed to.

Finally we have a bunch of function pointers for managing the module's global
state. When we don't have any state (the size if ``-1``), we can set these all
to :c:data:`NULL`.

Making The Shared Object Importable
-----------------------------------

With our function and module defined, we need to tell CPython how to import our
module. To do that we need to define a single function with type
:c:macro:`PyMODINIT_FUNC` named ``PyInit_{name}`` where ``name`` is the name of
our module.

This function will be executed the first time someone writes ``import fib.fib``
from python. This can be thought of as the code that runs at "module scope" in a
normal Python file.

At the end of our function we need to return the newly created module. To
actually create a :c:type:`PyObject*` from a :c:type:`PyModule_Def` we can use
:c:func:`PyModule_Create`.

An example :c:macro:`PyMODINIT_FUNC` for our fib module looks like:

.. code-block:: c

   PyMODINIT_FUNC
   PyInit_fib(void)
   {
       return PyModule_Create(&fib_module);
   }
