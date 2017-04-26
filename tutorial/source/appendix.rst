Appendix
========

Glossary
--------

.. _new-reference:

New Reference
~~~~~~~~~~~~~

A new reference is a :c:type:`PyObject*` which must be managed by the user. The
user must ensure that :c:func:`Py_DECREF` is called when the object is no longer
needed.

.. _borrowed-reference:

Borrowed Reference
~~~~~~~~~~~~~~~~~~

A borrowed reference is a :c:type:`PyObject*` which is not owned. Users should
*not* call :c:func:`Py_DECREF` when they are done with this object. A borrowed
reference can become a :ref:`new reference <new-reference>` by calling
:c:func:`Py_INCREF` on the it.

Reference Counting
------------------

.. _ref-count:

Reference Count
~~~~~~~~~~~~~~~

The number of refererants of a given object. This is the count of objects which
refer to the given object.

When an object is created, its reference count starts at one, which is the
reference owned by the creator. The reference count can be increased with
:c:func:`Py_INCREF` or decreased with :c:func:`Py_DECREF`. As soon as the
reference count reaches zero, the object is no longer needed and it will be
deallocated.


``Py_INCREF``
~~~~~~~~~~~~~

.. c:function:: void Py_INCREF(PyObject* ob)

   The ``Py_INCREF`` macro increments the :ref:`reference count <ref-count>` of
   an object by one.

   :param PyObject* ob: The object to increment the reference count of.

``Py_XINCREF``
~~~~~~~~~~~~~~

.. c:function:: void Py_XINCREF(PyObject* ob)

   The ``Py_INCREF`` macro increments the :ref:`reference count <ref-count>` of
   an object if ``ob`` is not ``NULL``.

   :param PyObject* ob: The object to increment the reference count of.

``Py_DECREF``
~~~~~~~~~~~~~

.. c:function:: void Py_DECREF(PyObject* ob)

   The ``Py_DECREF`` macro decrements the :ref:`reference count <ref-count>` of
   an object by one.

   :param PyObject* ob: The object to decrement the reference count of.

``Py_XDECREF``
~~~~~~~~~~~~~~

.. c:function:: void Py_XDECREF(PyObject* ob)

   The ``Py_XDECREF`` macro decrements the :ref:`reference count <ref-count>` of
   an object if ``ob`` is not ``NULL``.

   :param PyObject* ob: The object to decrement the reference count of.

``Py_CLEAR``
~~~~~~~~~~~~

.. c:function:: void Py_CLEAR(PyObject* ob)

   The ``Py_CLEAR`` macro decrements the :ref:`reference count <ref-count>` of
   an object and then sets the input :c:type:`PyObject*` to ``NULL``.

   :param PyObject* ob: The object to decrement the reference count of and set
                        to ``NULL``.

CPython Structures
------------------

``Py_ssize_t``
~~~~~~~~~~~~~~

.. c:type:: Py_ssize_t

   A ``Py_ssize_`` is a signed integral type used to hold counts and object
   sizes. On many platforms this is an alias of ``ssize_t``.

``PyObject``
~~~~~~~~~~~~

.. c:type:: PyObject

   https://docs.python.org/3.6/c-api/structures.html#c.PyObject

   ``PyObject`` is the structure which holds Python values. The definition looks
   like:

.. c:member:: Py_ssize_t PyObject.ob_refcnt

   The object's :ref:`reference count <ref-count>`.

.. c:member:: PyTypeObject* PyObject.ob_type

   The object's type as a Python object.

.. note::

   Users should not access these fields directly, instead go through the API
   macros: :c:func:`Py_REFCNT` and :c:func:`Py_TYPE`.

``PyTypeObject``
~~~~~~~~~~~~~~~~

.. c:type:: PyTypeObject

   https://docs.python.org/3/c-api/type.html

   ``PyTypeObject`` is the structure which holds Python ``type`` objects.

Casting Rules
`````````````

A ``PyTypeObject*`` can safely be cast to a :c:type:`PyObject*`.

:c:type:`PyObject*`\s can be cast to ``PyTypeObject*``\s only after a
``PyType_Check``.


``PyLongObject``
~~~~~~~~~~~~~~~~

.. c:type:: PyLongObject

   https://docs.python.org/3/c-api/long.html

   ``PyLongObject`` is the structure which holds Python ``int`` objects. This is
   called a ``PyLongObject`` as a hold over from when arbitrary width integer
   was called a ``long`` object in Python 2.

Casting Rules
`````````````

A ``PyLongObject*`` can safely be cast to a :c:type:`PyObject*`.

:c:type:`PyObject*`\s can be cast to ``PyLongObject*``\s only after a
``PyLong_Check``.


``PyMethodDef``
~~~~~~~~~~~~~~~

.. c:type:: PyMethodDef

   https://docs.python.org/3.6/c-api/structures.html#c.PyMethodDef

   Structure used to describe a method of an extension type. This structure has
   four fields.

.. c:member:: char* PyMethodDef.ml_name

   The name of the method as a C string.

.. c:member:: PyCFunction PyMethodDef.ml_meth

   A pointer to the C implmenetation of the method.

.. c:member:: int PyMethodDef.ml_flags

   Flag bits indicating how to call :c:member:`~PyMethodDef.ml_meth` should be
   called or bound to a class.

.. c:macro:: METH_VARARGS

   This is the typical calling convention, where the methods have the type
   PyCFunction. The function expects two :c:func:`PyObject*` values. The first
   one is the ``self`` object for methods; for module functions, it is the
   module object. The second parameter (often called ``args``) is a tuple object
   representing all arguments. This parameter is typically processed using
   :c:func:`PyArg_ParseTuple` or :c:func:`PyArg_UnpackTuple`.

.. c:macro:: METH_KEYWORDS

   The function expects three parameters: ``self``, ``args``, and a
   ``dictionary`` of all the keyword arguments. The flag must be combined with
   METH_VARARGS, and the parameters are typically processed using
   :c:func:`PyArg_ParseTupleAndKeywords`.

.. c:macro:: METH_NOARGS

   Methods without parameters don’t need to check whether arguments are given if
   they are listed with the ``METH_NOARGS`` flag. They need to be of type
   :c:type:`PyCFunction`. The first parameter is typically named ``self`` and
   will hold a reference to the module or object instance. In all cases the
   second parameter will be ``NULL``.

.. c:macro:: METH_O

   Methods with a single object argument can be listed with the ``METH_O`` flag,
   instead of invoking :c:func:`PyArg_ParseTuple` with a "O" argument. They have
   the type :c:type:`PyCFunction`, with the ``self`` parameter, and a
   :c:type:`PyObject*` parameter representing the single argument.

.. c:macro:: METH_CLASS

   Indicates that this method should be bound as a ``classmethod`` instead of an
   instance method. The first argument will be the class object.

.. c:macro:: METH_STATIC

   Indicates that this method should be bound as a ``staticmethod`` instead of
   an instance method. The first argument will always be ``NULL``.

.. c:macro:: METH_COEXIST

   Allow a method to have the same name as an automatically generated
   wrapper. This is useful for providing a specialized version of data model
   model method like ``__contains__`` when implementing the C-API version like
   ``sq_contains``.

   .. note::

      This flag is not used often and can be ignored when writing most
      extensions.

.. c:member:: char* PyMethodDef.ml_doc

   The contents of the method's docstring as a C string. If this is ``NULL``,
   the docstring will be ``None`` in Python. This should be created with
   :c:func:`PyDoc_STRVAR`.

``PyCFunction``
~~~~~~~~~~~~~~~

.. c:type:: PyCFunction

   Type of the functions used to implement most Python callables in C. Functions
   of this type take two :c:type:`PyObject*` parameters and return one such
   value. If the return value is ``NULL``, an exception shall have been set. If
   not ``NULL``, the return value is interpreted as the return value of the
   function as exposed in Python. The function must return a new reference.

``PyModuleDef``
~~~~~~~~~~~~~~~

.. c:type:: PyModuleDef

   A specification of a Python module object.

.. c:member:: PyModuleDef_Base PyModuleDef.m_base

   The part of the module definition managed by CPython. Always initialize this
   with ``PyModuleDef_HEAD_INIT``.

.. c:member:: char* PyModuleDef.m_name

   The name of the module as a C string.

.. c:member:: char* PyModuleDef.m_doc

   The docstring of the module as a C string. If set to ``NULL`` this will be
   ``None`` in Python This should be created with :c:func:`PyDoc_STRVAR`.

.. c:member:: Py_ssize_t PyModuleDef.m_size

   The size of the module's global state. If set to -1 this will not allocate
   any space for global state.

.. c:member:: PyMethodDef* PyModuleDef.m_methods

   A ``NULL`` terminated array of methods to put in this module.

.. c:member:: PyModuleDef_Slot* PyModuleDef.m_slots

   A ``NULL`` terminated array of slots for using multi-phase
   initialization. This is not used in this tutorial and can be set to ``NULL``.

.. c:member:: traverseproc PyModuleDef.m_traverse

   The function used for traversing the global state allocated with
   :c:member:`~PyModuleDef.m_size`. This can be ``NULL`` if not needed.

.. c:member:: inquiry PyModuleDef.m_clear

   The function used for clearing the global state allocated with
   :c:member:`~PyModuleDef.m_size`. This can be ``NULL`` if not needed.

.. c:member:: freefunc PyModuleDef.m_free

   The function used for freeing the global state allocated with
   :c:member:`~PyModuleDef.m_size`. This can be ``NULL`` if not needed.

``PyMODINIT_FUNC``
~~~~~~~~~~~~~~~~~~

.. c:macro:: PyMODINIT_FUNC

   The type of the module initialization function. This function should return a
   new module object or ``NULL`` if the file cannot be imported. The function
   needs to be named: ``PyInit_{name}`` where ``name`` is the name of the
   module.

Global Sentinels
----------------

``Py_None``
~~~~~~~~~~~

.. c:var:: PyObject* Py_None

   A global reference to ``None``.

.. c:macro:: Py_RETURN_NONE

   Shorthand for:

   .. code-block:: c

      Py_INCREF(Py_None);
      return Py_None;

   This is useful because people often forget that they need to call
   :c:func:`Py_INCREF` on :c:data:`Py_None` even though it is a global object.

``Py_True``
~~~~~~~~~~~

.. c:var:: PyObject* Py_True

   A global reference to ``True``.

``Py_False``
~~~~~~~~~~~~

.. c:var:: PyObject* Py_False

   A global reference to ``False``.

CPython Functions and Macros
----------------------------

``Py_REFCNT``
~~~~~~~~~~~~~

.. c:function:: Py_ssize_t Py_REFCNT(ob)

   https://docs.python.org/3.6/c-api/structures.html#c.Py_REFCNT

   Access the :ref:`reference count <ref-count>` of an object. This expands to:
   ``(((PyObject*) (o))->ob_refcnt)``.

   :param ob: The object to get the reference count of.
   :return: The reference count of ``ob``.

``Py_TYPE``
~~~~~~~~~~~

.. c:function:: PyTypeObject* Py_TYPE(ob)

   https://docs.python.org/3.6/c-api/structures.html#c.Py_TYPE

   Access the Python type of an object as a :c:type:`PyTypeObject*`. This
   returns a :ref:`borrowed reference <borrowed-reference>`. This macro expands
   to: ``(((PyObject*) (o))->ob_type)``

   :param ob: The object to get the type of.
   :return: A borrowed reference to the type of ``ob``.

``PyDoc_STRVAR``
~~~~~~~~~~~~~~~~

.. c:function:: void PyDoc_STRVAR(name, docstring)

   A macro for defining docstrings. This is used to support ``./configure
   --without-doc-strings``. This macro expands to something like:

   .. code-block:: c

      #ifndef PyDoc_STR
      #define PyDoc_VAR(name)         static char name[]
      #define PyDoc_STR(str)          (str)
      #define PyDoc_STRVAR(name, str) PyDoc_VAR(name) = PyDoc_STR(str)
      #endif

``PyLong_AsUnsignedLong``
~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: unsigned long PyLong_AsUnsignedLong(PyObject* ob)

   Convert a ``PyObject*`` of type :c:type:`PyLongObject*` to an ``unsigned
   long``. If ``ob`` is not a ``long`` object, an exception is raised.

   :param PyObject* ob: The object to convert.
   :return: ``ob`` as an ``unsigned long``.

``PyLong_FromUnsignedLong``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: PyObject* PyLong_FromUnsignedLong(unsigned long l)

   Convert an ``unsigned long`` into a :c:type:`PyObject*`. If the object cannot
   be allocated an exception is raised.

   :param l: The unsigned long to convert to a pyobjectptr.
   :return: a :ref:`new reference <new-reference>` to ``l`` as a Python
            object.

``PyModule_Create``
~~~~~~~~~~~~~~~~~~~

.. c:function:: PyObject* PyModule_Create(PyModuleDef* def)

   Create a new Python module object from a :c:type:`PyModuleDef*`.

``PyObject_Repr``
~~~~~~~~~~~~~~~~~

.. c:function:: PyObject* PyObject_Repr(PyObject* ob)

   Get the string representation of ``ob``. This is the same as ``repr(ob)`` in
   Python.

   :param PyObject* ob: The object to repr.
   :return: A :ref:`new reference <new-reference>` to the string representation
            of ``ob``.

``PyObject_GetAttr``
~~~~~~~~~~~~~~~~~~~~

.. c:function:: PyObject* PyObject_GetAttr(PyObject* ob, PyObject* attr_name)

   Lookup an attribute on a Python object. This is the same as ``getattr(ob,
   attr_name)`` in Python.

   :param PyObject* ob: The object to lookup the attribute on.
   :param PyObject* attr_name: The name of the attribute to lookup.
   :return: The attribute name ``attr_name`` on ``ob`` or ``NULL`` with an
            exception set if the attribute doesn't exist.


``PyObject_SetAttr``
~~~~~~~~~~~~~~~~~~~~

.. c:function:: int PyObject_SetAttr(PyObject* ob, PyObject* attr_name, PyObject* value)

   Set an attribute on a Python object. This is the same as ``setattr(ob,
   attr_name, value)`` in Python.

   :param PyObject* ob: The object to set the attribute on.
   :param PyObject* attr_name: The name of the attribute to set.
   :param PyObject* value: The value of the attribute to set.
   :return: True with an exception set if an error occurred, otherwise False.

``PyObject_IsTrue``
~~~~~~~~~~~~~~~~~~~

.. c:function:: int PyObject_IsTrue(PyObject* ob)

   Check the truthiness of an object. This is the same as ``bool(ob)`` in
   Python.

   .. note::

      This is not the same as ``ob is True`` in Python, this name is slightly
      confusing.

   :param PyObject* ob: The object to check the truthiness of.
   :return: True if the object is truthy, False if the object is falsey, -1 with
            an exception raised if an error occurred.

``PyObject_RichCompare``
~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: PyObject* PyObject_RichCompareBool(PyObject* ob_1, PyObject* ob_2, int op_id)

   Compare ``ob_1`` and ``ob_2`` with some comparison operator.

   :param PyObject* ob_1: The first operand.
   :param PyObject* ob_2: The second operand.
   :param int op_id: The operator to use: {:c:macro:`Py_LT`, :c:macro:`Py_LE`,
                     :c:macro:`Py_EQ`, :c:macro:`Py_NE`, :c:macro:`Py_GT`,
                     :c:macro:`Py_GE`}.
   :return: The result of the operator.


``PyObject_RichCompareBool``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: int PyObject_RichCompareBool(PyObject* ob_1, PyObject* ob_2, int op_id)

   Compare ``ob_1`` and ``ob_2`` with some comparison operator and convert the
   values to a C boolean.

   :param PyObject* ob_1: The first operand.
   :param PyObject* ob_2: The second operand.
   :param int op_id: The operator to use: {:c:macro:`Py_LT`, :c:macro:`Py_LE`,
                     :c:macro:`Py_EQ`, :c:macro:`Py_NE`, :c:macro:`Py_GT`,
                     :c:macro:`Py_GE`}.
   :return: The status of the operator or -1 with an exception set if an error
            occurred.

.. c:macro:: Py_LT

   ``ob_1 < ob_2``

.. c:macro:: Py_LE

   ``ob_1 <= ob_2``

.. c:macro:: Py_EQ

   ``ob_1 == ob_2``

   .. note::

      :c:func:`PyObject_RichCompareBool` will return True if ``ob_1 is ob_2``.

.. c:macro:: Py_NE

   ``ob_1 != ob_2``

   .. note::

      :c:func:`PyObject_RichCompareBool` will return False of ``ob_1 is ob_2``.

.. c:macro:: Py_GT

   ``ob_1 > ob_2``

.. c:macro:: Py_GE

   ``ob_1 >= ob_2``

``PyArg_ParseTupleAndKeywords``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: int PyArg_ParseTupleAndKeywords(PyObject* args, PyObject* kwargs, const char* format, char** keywords, ...)

   Parse the argument tuple and dictionary for a :c:type:`PyCFunction`.

   :param PyObject* args: The argument tuple passed to the
                          :c:type:`PyCFunction`.
   :param PyObject* kwargs: The keyword argument dictionary passed to the
                            :c:type:`PyCFunction`. This *can* be ``NULL``.
   :param const char* format: The format string. see :ref:`format characters
                              <arg-format>` for more information.
   :param char** keywords: The names of the keyword arguments that this function
                           accepts as a ``NULL`` terminated array.
   :param ...: Variadic values based on ``format``.
   :return: True with an exception set if an error occurred, otherwise False.


Example
```````

The following example defines a function called ``function_name`` which accepts
three arguments:

- ``a``: A Python ``str`` object to be converted into a ``char*``.
- ``b``: A Python ``int`` object to be converted into an ``int``.
- ``c``: An optional arbitrary Python object.

.. code-block:: c

   static PyObject*
   function_name(PyObject* self, PyObject* args, PyObject* kwargs)
   {
       /* the names of the arguments */
       static char* keywords[] = {"a", "b", "c", NULL};

       /* C declarations for our arguments */
       char* string;
       int integer;
       PyObject* object = NULL;

       if (PyArg_ParseTupleAndKeywords(args,
                                       kwargs,
                                       "si|O:function_name",
                                       &string,   /* s /*
                                       &integer,  /* i */
                                       &object    /* O */)) {
           /* failed to parse arguments, an error is set */
           return NULL;
       }

       if (!object) {
           /* ``c`` was not passed, set its default value here */
       }

       /* rest */
   }

.. _arg-format:

Format Characters
`````````````````

Below is a subset of commonly used format characters, see
https://docs.python.org/3/c-api/arg.html#strings-and-buffers for a full list.

``s`` (str) [const char*]
'''''''''''''''''''''''''

Accept a ``str`` argument as a ``char*``. A reference to a ``char*`` should
appear in the variadic argument list at this index.

``z`` (str) [const char*]
'''''''''''''''''''''''''

Accept a ``str`` argument as a ``char*``. A reference to a ``char*`` should
appear in the variadic argument list at this index. This argument can also be
``None`` in which case the pointer will be ``NULL``.

``b`` (int) [unsigned char]
'''''''''''''''''''''''''''

Accept an ``int`` argument as an ``unsigned char``. A reference to an ``unsigned
char`` should appear in the variadic argument list at this index.

``h`` (int) [short]
'''''''''''''''''''

Accept an ``int`` argument as a ``short``. A reference to a ``short`` should
appear in the variadic argument list at this index.

``i`` (int) [int]
'''''''''''''''''

Accept an ``int`` argument as an ``int``. A reference to an ``int`` should
appear in the variadic argument list at this index.

``i`` (int) [long]
''''''''''''''''''

Accept an ``int`` argument as a ``long``. A reference to a ``long`` should
appear in the variadic argument list at this index.

``C`` (str of length 1) [int]
'''''''''''''''''''''''''''''

Accept a ``str`` of length 1 argument as an ``int``. A reference to an ``int``
should appear in the variadic argument list at this index.

``f`` (float) [float]
'''''''''''''''''''''

Accept a ``float`` argument as a ``float``. A reference to a ``float`` should
appear in the variadic argument list at this index.

``d`` (float) [double]
''''''''''''''''''''''

Accept a ``float`` argument as a ``double``. A reference to a ``double`` should
appear in the variadic argument list at this index.

``O`` (object) [:c:type:`PyObject*`\]
'''''''''''''''''''''''''''''''''''''

Accept an object argument as a :c:type:`PyObject*`. This is a :ref:`borrowed
reference <borrowed-reference>`. A reference to a :c:type:`PyObject*` should
appear in the variadic argument list at this index.

``O!`` (object) [:c:type:`PyTypeObject*`, :c:type:`PyObject*`\]
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Accept an argument as a :c:type:`PyObject*`. This object must be of a particular
Python type. This is a :ref:`borrowed reference <borrowed-reference>`. This
format requires two values in the variadic argument list:

1. A :c:type:`PyTypeObject*` to check the type of the argument against. Parsing
   will fail if the object is not an instance of this type.
2. A reference to a :c:type:`PyObject*` to write the result.

``p`` (bool) [int]
''''''''''''''''''

Accept any argument and check the truthiness of the value. A reference to an
``int`` should appear in the variadic argument list at this index. This is like
accepting an object as ``O`` and then using :c:func:`PyObject_IsTrue`.

.. warning::

   The CPython docs mention converters for ``unsigned`` integral types which do
   *not* do overflow checking. These converters should not be used because they
   fail to handle negative integers. The proper way to handle these values is to
   accept them as ``O`` and use one of the ``PyLong_As*`` conversion functions.

Special Characters
``````````````````

``|``
'''''

All arguments following a pipe are optional. If an argument is not passed, the
value of the reference in the variadic argument list is unchanged.

``$``
'''''

All arguments following a dollar sign are keyword only arguments.

.. note::

   The CPython docs say that this can only appear after a ``|``; however, this
   is not actually true. You may have required keyword only arguments.

``:``
'''''

This marks the end of the format list. Any text after this is used as the name
of the function when generating error messages.

.. _number-api:

Number API
----------

The Number API is sort of like the ``operator`` module in Python.

For each function ``PyNumber_Op`` there is a matching ``PyNumber_InPlaceOp``
which is an augmented assignment version. For example: ``PyNumber_InPlaceAdd(a,
b)`` is the same as ``a += b; return a``. This still returns a :ref:`new
reference <new-reference>` to ``a`` which the caller owns.

.. c:function:: PyObject* PyNumber_Add(PyObject* a, PyObject* b)

   :return: A new reference to ``a + b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Subtract(PyObject* a, PyObject* b)

   :return: A new reference to ``a - b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Subtract(PyObject* a, PyObject* b)

   :return: A new reference to ``a * b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_FloorDivide(PyObject* a, PyObject* b)

   :return: A new reference to ``a // b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_TrueDivide(PyObject* a, PyObject* b)

   :return: A new reference to ``a / b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Remainder(PyObject* a, PyObject* b)

   :return: A new reference to ``a % b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Divmod(PyObject* a, PyObject* b)

   :return: A new reference to ``divmod(a, b)`` or ``NULL`` with an exception
            set.

.. c:function:: PyObject* PyNumber_Power(PyObject* a, PyObject* b, PyObject* c)

   :param PyObject* a: The base.
   :param PyObject* b: The exponent.
   :param PyObject* c: Number to take the exponent modulo. If provided, this
                       function is like ``(a ** b) % c``. To ignore this value
                       pass :c:data:`Py_None`.
   :return: A new reference to ``pow(a, b, c)`` or ``NULL`` with an exception
            set.

.. c:function:: PyObject* PyNumber_Negative(PyObject* a)

   :return: A new reference to ``-a`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Positive(PyObject* a)

   :return: A new reference to ``+a`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Absolute(PyObject* a)

   :return: A new reference to ``abs(a)`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Invert(PyObject* a)

   :return: A new reference to ``~a`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Lshift(PyObject* a, PyObject* b)

   :return: A new reference to ``a << b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Rshift(PyObject* a, PyObject* b)

   :return: A new reference to ``a >> b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_And(PyObject* a, PyObject* b)

   .. note::

      This is bitwise ``and``, not boolean ``and``.

   :return: A new reference to ``a & b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Xor(PyObject* a, PyObject* b)

   :return: A new reference to ``a ^ b`` or ``NULL`` with an exception set.

.. c:function:: PyObject* PyNumber_Or(PyObject* a, PyObject* b)

   .. note::

      This is bitwise ``or``, not boolean ``or``.

   :return: A new reference to ``a | b`` or ``NULL`` with an exception set.

Error Handling
--------------

.. c:function:: void PyErr_SetString(PyObject* type, const char* message)

   https://docs.python.org/3.6/c-api/exceptions.html#c.PyErr_SetString

   Raise an exception of the given type with the given message.

   :param PyObject* type: The exception type to raise.
   :param const char* message: The exception message.

.. c:function:: PyObject* PyErr_Format(PyObject* type, const char* format, ...)

   https://docs.python.org/3.6/c-api/exceptions.html#c.PyErr_Format

   Raise an exception of the given type with a message created from a format
   string and values.

   :param PyObject* type: The exception type to raise.
   :param const char* format: The exception format string.
   :param ...: The values to format into ``format``.
   :return: Always ``NULL``.

.. c:function:: PyObject* PyErr_NoMemory()

   https://docs.python.org/3.6/c-api/exceptions.html#c.PyErr_NoMemory

   Raise an exception that indicates that memory could not be allocated.

   :return: Always ``NULL``.

.. c:function:: PyObject* PyErr_Occurred()

   https://docs.python.org/3.6/c-api/exceptions.html#c.PyErr_Occurred

   Return a :ref:`borrowed reference <borrowed-reference>` to the type of the
   currently raised exception. If no exception is raised, return ``NULL``.

   This should just be used to check if an exception is raised, do not compare
   this value to exception types. To compare exceptions to an exception type
   use: :c:func:`PyErr_ExceptionMatches`.

.. c:function:: int PyErr_ExceptionMatches(PyObject* exc)

   https://docs.python.org/3.6/c-api/exceptions.html#c.PyErr_ExceptionMatches

   Compare the currently raised exception type to ``exc``. Return ``true`` if
   the currently raised exception is an instance of ``exc``.

   :param PyObject* exc: The exception type to compare the currently raised
                         exception to.
   :return: ``1`` if the current exception is a subclass of ``exc``, ``0`` if it
            is not a subclass of ``exc``, and ``-1`` if an error occurs.

   .. note::

      This can only be called if an exception is currently raised. This can be
      checked with :c:func:`PyErr_Occurred`.
