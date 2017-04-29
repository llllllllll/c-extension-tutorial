Building and Importing
======================

Compiling the Module
--------------------

The most common way to build CPython extension modules is to use setuptools and
a ``setup.py`` file. We start with a normal setuptools ``setup.py`` file, for
example:

.. code-block:: python
   :emphasize-lines: 14

   from setuptools import setup, find_packages


   setup(
       name='fib',
       version='0.1.0',
       packages=find_packages(),
       license='GPL-2',
       classifiers=[
           'Development Status :: 3 - Alpha',
           'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',
           'Natural Language :: English',
           'Programming Language :: Python :: 3 :: Only',
           'Programming Language :: Python :: Implementation :: CPython',
       ],
   )


This defines a new package called ``fib`` with version '0.1.0' and some other
metadata.

.. note::

   In the classifiers we indicate:

   ``Programming Language :: Python :: Implementation :: CPython``

   This indicates that our package requires CPython extension modules or
   implementation details and would not work on PyPy, Jython, or other Python
   implementations.

With the standard boilerplate in place we need to add the C extension specific
parts. Start by importing the ``Extension`` type from ``setuptools``:

.. code-block:: python

   from setuptools import setup, find_packages, Extension

Next, we need to add a list of all of the extension modules we need build to our
call to ``setup``:

.. code-block:: python

   setup(
       ...,  # the arguments from before
       ext_modules=[
           Extension(
               # the qualified name of the extension module to build
               'fib.fib',
               # the files to compile into our module relative to ``setup.py``
               ['fib/fib.c'],
           ),
       ],
   )

The ``Extension`` class makes sure we get the correct CPython headers and flags
which were used to build the CPython invoking ``setup.py``. We can customize the
build process with arguments to ``Extension``, but the default is enough to get
us started.

Building
--------

Let's make sure our build environment is working.

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

What happens if ``n`` is not actually an :c:type:`int` object? Try ``fib('a')``
or another non-int object.

If ``C-c`` doesn't kill the session, you might need to use ``C-z`` and then ``$
kill %1``.

Why Did this Hang?
------------------

When ``n`` is not an integer object, :c:func:`PyLong_AsUnsignedLong` raises an
exception and returns ``(unsigned long) -1`` which is
:c:macro:`UNSIGNED_LONG_MAX`. We ignore the error and enter the Fibonacci
function's loop which tries computing the ``18446744073709551615``\th Fibonacci
number which will take a very long time.
