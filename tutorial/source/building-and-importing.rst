Building and Importing
======================

Building
--------

To start, let's make sure our build environment is working.

Run the following commands from the repo root:

.. code-block:: bash

   $ cd exercises/fib
   $ python setup.py build_ext --inplace


``python setup.py build_ext --inplace`` uses the ``setup.py`` to build our
extension modules. The ``--inplace`` flag moves the compiled shared objects into
the source tree so that they are in the Python search path.

Importing and Using
-------------------

Now that the modules are built and moved to the search path, we can try to
import and use the code in a normal repl. From the ``fib`` directory startup
``ipython`` and try some of the following expressions:

.. code-block:: IPython

   In [1]: from fib import fib

   In [2]: fib(1)
   Out[2]: 1

   In [3]: fib(2)
   Out[3]: 1

   In [4]: fib(3)
   Out[4]: 2

   In [5]: fib(10)
   Out[5]: 55

Invalid Arguments
-----------------

Think back to our definition of ``pyfib`` in C. We started with:

.. code-block:: c

   static PyObject *
   pyfib(PyObject *self, PyObject *n)
   {
       unsigned long as_unsigned_long = PyLong_AsUnsignedLong(n);
       /* ... */
   }

What happens if ``n`` is not actually an ``int`` object? Try ``fib('a')`` or
another non-int object.

If ``C-c`` doesn't kill the session, you might need to use ``C-z`` and then ``$
kill %1``.

Why Did this Hang?
------------------

When ``n`` is not an integer object, :c:func:`PyLong_AsUnsignedLong` raises an
exception and returns ``(unsigned long) -1`` which is
``MAX_UNSIGNED_LONG``. We ignore the error and enter the Fibonacci function's
loop which tries computing the ``18446744073709551615``\th Fibonacci number
which will take a very long time.
