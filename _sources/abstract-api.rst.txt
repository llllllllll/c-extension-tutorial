Abstract Object API
===================

So far we have talked about how to coerce Python objects to and from ``unsigned
long`` objects; however, one of the benefits of Python is that it is a dynamic
language. Being dynamic means we can program to an object's interface and write
code that accepts any object that matches that interface. This pattern is often
called "duck typing".

The C API also allows us to write code that is generic in this way by using the
Abstract Object API. The Abstract Object API is broken up into a couple parts
based on common Python protocols, for example, the mapping protocol or the
iterator protocol. Some functions can act on any type, for example:
:c:func:`PyObject_Repr`

There are performance trade offs to consider when using the Abstract Object
API. The Abstract Object API requires a lot of runtime dispatching and object
allocations so it will often be slower than the equivalent code using a more
specific object's API; but, it allows us to write functions which behave more
like normal Python functions.

Generic Object Functions
------------------------

Most of the builtin Python functions have an Abstract Object API
equivalent. Generally the name is the same except it is PyObject_ClassCase
instead of alllowercase.

- :c:func:`PyObject_GetAttr`
- :c:func:`PyObject_SetAttr`
- :c:func:`PyObject_Repr`

Comparison
~~~~~~~~~~

A weird quirk of the CPython API is that all of the comparison operators (``>``,
``>=``, ``==``, etc...) are grouped into a single function called
:c:func:`PyObject_RichCompare` instead of different functions like in
Python. This is probably done to save space in the :c:type:`PyTypeObject`
struct.

This function accepts the two operands followed by a macro which specifies the
operator to use. For example, to call ``a < b`` in the CPython API you would
write:

.. code-block:: c

   PyObject* a;  /* ... */
   PyObject* b;  /* ... */

   PyObject* result = PyObject_RichCompare(a, b, Py_LE);
   if (!result) {
       /* error handling */
   }
   /* do stuff with ``result`` */
   Py_DECREF(result);

:c:func:`PyObject_RichCompare` can invoke arbitrary Python code so we need to
check for errors. This will also raise an error if ``a`` and ``b`` cannot be
compared because they are incompatible types.

Because it is common to want the result of a comparison as a boolean value,
there is a helper function called :c:func:`PyObject_RichCompareBool` which
returns a C :c:type:`int` instead of a :c:type:`PyObject*`. This saves us the
hassle of worrying about cleaning up the reference to ``result``. We could
rewrite the above example with :c:func:`PyObject_RichCompareBool` as:

.. code-block:: c

   PyObject* a;  /* ... */
   PyObject* b;  /* ... */

   int result = PyObject_RichCompareBool(a, b, Py_LE);
   if (result < 0) {
       /* error handling */
   }

The function can still raise an exception, so we need to check for values less
than 0 which signals that an error occurred.

Number Protocol
---------------

Unlike comparisons, there are different functions for all of the numeric
operators. These are mostly named ``PyNumber_{Operator}``, for example:

- :c:func:`PyNumber_Add`
- :c:func:`PyNumber_Subtract`
- :c:func:`PyNumber_Multiply`

and so on.

Like the rest of the Abstract Object API, these functions are generic on their
input and return new :c:type:`PyObject*`\s.

For a more complete list of Number API functions, see :ref:`number-api`.

Using the Number API in ``fib``
-------------------------------

One nice thing about Python :c:type:`int` objects is that they can hold
arbitrarily large integers. This is not true for :c:type:`unsigned long` values
which can store at most ``2 ** 64 - 1``. The Fibonacci sequence grows quickly
and we will run out of room to store the results if we represent it as an
:c:type:`unsigned long`.

Open up ``fib.c`` and change it so that we store ``a`` and ``b`` in
:c:type:`PyObject*` values. Use the Number API functions to do arithmetic. We
can continue to hold ``n`` in an :c:type:`unsigned long` because that is a
reasonable limit for the number of Fibonacci numbers we can compute.

.. note::

   Remember to check for errors!

Extra Work
~~~~~~~~~~

If you complete the exercise early, try to optimize ``fib`` by only using the
boxed integer values when we know ``fib(n)`` will overflow.

This technique of having a typed fast path for common inputs with a generic
fallback path dramatically speed up extensions.
