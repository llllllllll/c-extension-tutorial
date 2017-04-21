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
:ref:`pyobjectptr`\s to and from C ``unsigned long`` objects to invoke this from
Python.

:ref:`py-long-object`
---------------------

Python ``int`` objects are concretely typed as a ``PyLongObject``.

.. note::

   Python ``int`` objects are called ``PyLongObject`` in the C API as a holdover
   from when ``int`` and ``long`` were different in Python 2. In Python 3:
   ``int`` is arbitrarily sized like the old Python 2 ``long``.

Adapting Long Objects
---------------------

For many primitive C types, CPython provides functions to convert to and from
Python objects. For ``unsigned long``, we can use
:c:func:`PyLong_FromUnsignedLong` and :c:func:`PyLong_AsUnsignedLong` to convert
to and from C unsigned long objects.

We can then write a wrapping function to do this conversion for us:

.. code-block:: c

   static PyObject *
   pyfib(PyObject *self, PyObject *n)
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
   ``PyLongObject*``. We will get to error handling later.

Creating a Python Callable Object
---------------------------------

Given our wrapping function, we still need a way to pass the function to Python
to be called. To do this, we need to associate some extra metadata with our C
function. This metadata is stored along with the function in a
:c:type:`PyMethodDef` structure.

This structure defines the name of the function as it will appear in Python, the
pointer to the C implementation, information about how to invoke the function,
and finally the docstring.

A ``PyMethodDef`` for our ``pyfib`` function looks like:

.. code-block:: c

   PyDOC_STRVAR(fib_doc, "computes the nth Fibonacci number);
   PyMethodDef fib_method = {"fib",
                             (PyCFunction) pyfib,
                             METH_O,
                             fib_doc};
