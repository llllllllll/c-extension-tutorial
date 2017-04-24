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
