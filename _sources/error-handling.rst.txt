Error Handling
==============

The C programming language does not have exceptions in the same way that Python
does. In C, all functions have a single return value which needs to encode the
return value as well as the validity of the result.

Global Exception Indicator
--------------------------

CPython uses three variables to store the state of the current exception. These
hold the type of the current exception, the value of the current exception, and
the Python traceback of the current exception. This is like an enhanced
``errno``.

These values can be set or queried with the ``PyErr_*`` family of functions.

Propagating Errors
------------------

If a C API function raises an error, the exception indicator will be set an a
value will be returned which indicates that a failure occurred. As a consumer of
an API function, you need to explicitly check for this error return value and
return a value that indicates that your function has failed as well.

:c:data:`NULL` :c:type:`PyObject*`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In the CPython API, a :c:data:`NULL` value is never valid for a
:c:type:`PyObject*` so it is used to signal that an error has occurred. For
example: :c:func:`PyLong_FromUnsignedLong` returns a :c:type:`PyObject*`;
however, when memory cannot be allocated a :c:data:`PyExc_MemoryError` is set
and :c:data:`NULL` is returned.

This is the most common error sentinel and is used for
:c:type:`PyCFunction`\s.

Boolean Values
~~~~~~~~~~~~~~

If a function does not return a :c:type:`PyObject*` it needs to specify a new
error sentinel. A common case is a function which returns a boolean. In The C
API, these functions will actually return an :c:type:`int`, not ``bool``. This
allows for three states to be returned: ``1 = True``, ``0 = False``, and ``-1 =
Error``.

Other
~~~~~

In very special cases a function will need to explicitly call out a new sentinel
value. For example: :c:func:`PyLong_AsUnsignedLong` returns the value of its
argument as an unsigned long. No value in the codomain is unused so it specifies
that :c:macro:`UNSIGNED_LONG_MAX` will be returned and the user should check to
see if an exception was raised with :c:func:`PyErr_Occurred`.

Reference Counting
~~~~~~~~~~~~~~~~~~

It is important to remember to cleanup any resources or references when exiting
early for an exception. A common pattern is to use a ``goto error`` statement to
cleanup all of your references before returning :c:data:`NULL`.

Raising Exceptions
------------------

To explicitly set the error indicator we can use one of
:c:func:`PyErr_SetString` or :c:func:`PyErr_Format`. These functions take an
exception type and either a message or a message format string and raise the
given Python exception. After setting the exception, we need to clean up our
references and then return :c:data:`NULL` or some other sentinel to indicate
that our function failed.

There are also helpers for raising common exceptions like:
:c:func:`PyErr_NoMemory`.

Standard Exceptions
~~~~~~~~~~~~~~~~~~~

All of the builtin exceptions are accessible in C with a naming scheme of
``PyExc_{Name}`` where ``Name`` is the name as it appears in Python. For
example:

:c:data:`PyExc_IndexError` is the same as :exc:`IndexError` from within Python.

A full list of exceptions can be found here:
https://docs.python.org/3.6/c-api/exceptions.html#standard-exceptions

Adding Error Handling to ``fib``
--------------------------------

Now that we know about exception handling and propagation, lets try to guard
against invalid input in ``fib``.

Open up ``fib.c`` and add error handling around :c:func:`PyLong_AsUnsignedLong`
to properly propagate exceptions.
