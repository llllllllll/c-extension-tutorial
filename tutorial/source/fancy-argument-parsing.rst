Fancy Argument Parsing
======================

:c:macro:`METH_O` is convenient for quickly testing a function, but Python
programmers are used to more expressive argument passing. Python functions can
accept multiple arguments, have default value, or accept arguments by
keyword.

As a first pass, we could replace the :c:macro:`METH_O` in our
:c:type:`PyMethodDef` structure with :c:macro:`METH_VARARGS` |
:c:macro:`METH_KEYWORDS`. This tell CPython to pass us three arguments:

1. :c:type:`PyObject*` ``self``: The module or instance.
2. :c:type:`PyObject*` ``args``: A tuple of positional arguments.
3. :c:type:`PyObject*` ``kwargs``: A dictionary of keyword arguments or
   :c:data:`NULL` if no keyword arguments are provided. The dict can also be
   empty.

This is like defining a Python function like:

.. code-block:: python

   def f(self, *args, **kwargs):
       ...

We now have full control over how we parse arguments, however, this would make
it very tedious to implement common patterns. CPython has a helper function
called :c:func:`PyArg_ParseTupleAndKeywords` which can take our ``args``,
``kwargs``, argument names, and some signature, and parse out the values.

To define the signature of our function we use a format string sort of like the
C standard library function ``sscanf``. This string needs to encode the type of
all of our arguments and how to convert them to C values.

The simplest type we can express is ``O``, which accepts any Python object and
gives it to the function as a :c:type:`PyObject*` with no conversion applied. We
can rewrite our ``pyfib`` parsing using just this single conversion like:

.. code-block:: c

   static PyObject*
   pyfib(PyObject* self, PyObject* args, PyObject* kwargs) {
       /* the names of the arguments as a static array */
       static char* keywords[] = {"n", NULL};

       PyObject* n;

       if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", keywords, &n)) {
           /* the arguments passed don't correspond to the signature
              described */
           return NULL;
       }

       /* ... */
   }


We start by changing the signature to accept ``args`` and ``kwargs`` instead of
just ``n``. Next, we need to define the names of the parameter as we will call
them from Python. We store this information in a ``static char*[]`` so we don't
need to re-initialize it each time the function is called. Next, we call
:c:func:`PyArg_ParseTupleAndKeywords` with our ``args``, ``kwargs``. We give it
a format string of ``"O"`` which means our function accepts exactly one object
of any kind given to us as a :c:type:`PyObject*`. After the format string we
pass the names of the arguments which is the array ``{"n", NULL}``. The number
of entries in this array should match up with the number of arguments specified
in the format string. After the argument names is the address of our local
variable ``n``. If argument parsing succeeds, ``n`` will now be a :ref:`borrowed
reference <borrowed-reference>` to the single argument.

We also need to update our :c:type:`PyMethodDef` to look like:

.. code-block:: c

   PyMethodDef methods[] = {
       {"fib", (PyCFunction) pyfib, METH_VARARGS | METH_KEYWORDS, fib_doc},
       {NULL},
   };

This looks like a lot more work than just using :c:macro:`METH_O`, but now we
can call ``fib`` from Python as ``fib(n=n)`` instead of just passing ``n``
positionally.

Type Conversion
---------------

One nice feature of :c:func:`PyArg_ParseTupleAndKeywords` is that it can convert
:c:type:`PyObject*`\s into C types for us. This helps us generate better and
more standardized error messages when users provide values of unexpected types.

For example, we can write a simple integer add function using
:c:func:`PyArg_ParseTupleAndKeywords` by using the ``i`` format character. This
format character looks for a Python :c:type:`int` object as an input and will
convert to a C :c:type:`int` for us. Instead of passing the address of a
:c:func:`PyObject*` to hold the result, we pass the address of a C
:c:type:`int`. For example:

.. code-block:: c

   static PyObject*
   add(PyObject* self, PyObject* args, PyObject* kwargs)
   {
       static char* keywords[] = {"a", "b", NULL};

       int a;
       int b;

       if (!PyArg_ParseTupleAndKeywords(args,
                                        kwargs,
                                        "ii",
                                        keywords,
                                        &a,
                                        &b)) {
           return NULL;
       }

       return PyLong_FromLong(a + b);
   }

This is a silly example because we would normally just use ``+`` in Python, but
we can use it to see the error messages we get:

.. code-block:: ipython

   In [2]: add(1, 2)  # call with correct positional arguments
   Out[2]: 3

   In [3]: add(1.5, 2)  # ``a`` as a float
   ---------------------------------------------------------------------------
   TypeError                                 Traceback (most recent call last)
   <ipython-input-3-06d2968d6f5e> in <module>()
   ----> 1 add(1.5, 2)

   TypeError: integer argument expected, got float

   In [4]: add(a=1, b=2)  # call with correct keyword arguments
   Out[4]: 3

   In [5]: add(1, 2, 3)
   ---------------------------------------------------------------------------
   TypeError                                 Traceback (most recent call last)
   <ipython-input-5-f6721327c99d> in <module>()
   ----> 1 add(1, 2, 3)

   TypeError: function takes at most 2 arguments (3 given)

   In [6]: add(a=1, b=2, c=3)  # unknown keyword argument
   ---------------------------------------------------------------------------
   TypeError                                 Traceback (most recent call last)
   <ipython-input-6-c9aad4a5afdb> in <module>()
   ----> 1 add(a=1, b=2, c=3)

   TypeError: function takes at most 2 arguments (3 given)

   In [7]: add(1, 2, a=3)  # duplicate argument (positional and keyword)
   ---------------------------------------------------------------------------
   TypeError                                 Traceback (most recent call last)
   <ipython-input-7-a765fb2e71a3> in <module>()
   ----> 1 add(1, 2, a=3)

   TypeError: function takes at most 2 arguments (3 given)

   In [8]: add(1, b=2)  # call with mix of positional and keyword arguments
   Out[8]: 3

Naming our Function
-------------------

The default error messages are descriptive but are missing the name of our
function. For example:

.. code-block:: python

   TypeError: function takes at most 2 arguments (3 given)

This can be confusing to readers later. :c:func:`PyArg_ParseTupleAndKeywords`
can be told the name of the function in the format string by suffixing the type
signature with ``:name``. In our ``add`` function we can change the format
string from ``"ii"`` to ``"ii:add"``. Now, if we pass too many arguments we will
see:

.. code-block:: python

   TypeError: add() takes at most 2 arguments (3 given)

Optional Arguments
------------------

Another feature of Python is support for optional or default
arguments. :c:func:`PyArg_ParseTupleAndKeywords` also supports optional
arguments with the ``|`` special character. Any parameter types that follow a
pipe will be considered optional. If the argument is not provided, the value of
the reference will remain unchanged. For example, we could add a default value
of ``b=1`` in our ``add`` function:

.. code-block:: c

   static PyObject*
   add(PyObject* self, PyObject* args, PyObject* kwargs)
   {
       static char* keywords[] = {"a", "b", NULL};

       int a;
       int b = 1;

       if (!PyArg_ParseTupleAndKeywords(args,
                                        kwargs,
                                        "i|i",
                                        keywords,
                                        &a,
                                        &b)) {
           return NULL;
       }

       return PyLong_FromLong(a + b);
   }

When ``b`` is not provided either positionally or by keyword, the value of
``&b`` will remain unchanged after :c:func:`PyArg_ParseTupleAndKeywords` so it
will have a default value of 1.

We can still explicitly pass ``b`` to change this: for example:

.. code-block:: ipython

   In [2]: add(1)
   Out[2]: 2

   In [3]: add(2)
   Out[3]: 3

   In [4]: add(2, 2)
   Out[4]: 4

Keyword Only Arguments
----------------------

Python 3 added support for keyword only arguments. These are arguments which
*cannot* be passed positionally. These are useful for boolean arguments, or
uncommon flags because it enforces the caller must be descriptive at the call
site.

:c:func:`PyArg_ParseTupleAndKeywords` supports keyword only arguments with the
``$`` special character. Any parameter types following a dollar sign can only be
passed by keyword. For example, if we hated our users, we could require ``b`` be
passed by keyword by changing our format string to: ``i$i``. Keyword only
arguments may also be optional, this is done by putting the the ``$`` after a
``|``, for example: ``i|$i``.

Add Optional Arguments ``a`` and ``b`` to ``fib``
-------------------------------------------------

As an exercise, try adding two new arguments to ``fib``: ``a`` and ``b``. These
arguments should both default to 1 and are the seed values for the Fibonacci
sequence.

You may either accept the arguments as :c:type:`PyObject*` and use the
:ref:`Abstract Number API <number-api>` or accept them as C types and use normal
C arithmetic.

If you finish early, try to implement it using the other technique.
