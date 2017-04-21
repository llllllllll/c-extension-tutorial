C Level Representation of Python Structures
===========================================

In the C API, Python objects are represented by a :ref:`pyobjectptr`. A
:ref:`pyobjectptr` is a reference to an object which is managed by the CPython
interpreter.

The ``PyObject`` struct is defined as:

.. code-block:: c

   typedef struct {
        Py_ssize_t ob_refcnt;   /* object reference count */
        PyTypeObject* ob_type;  /* object type */
   };


The ``ob_type`` field is a pointer to the Python type for the object. This is
how objects communicate their runtime type. For example: if a given
:ref:`pyobjectptr` points to a unicode object, then the ``ob_type`` field will
be set to ``&PyUnicode_Type`` where ``PyUnicode_Type`` is the Python ``unicode``
type. This should be accessed through :c:func:`Py_TYPE`.

the ``ob_refcnt`` is the :ref:`ref-count` of the object. This is the number of
places where this object is being used. This should be accessed through
:c:func:`Py_REFCNT`.

Object Lifetime
---------------

CPython uses a reference counting garbage collection strategy. When an object is
created, its reference count starts at one, which is the reference owned by the
creator. The reference count can be increased with :ref:`py-incref` or decreased
with :ref:`py-decref`. As soon as the reference count reaches zero, the object
is no longer needed and it will be deallocated.

Concrete Types
--------------

:ref:`pyobjectptr_` is an abstract reference that can point to any type;
however, we eventually need to actually store information on an object. Subtypes
of ``object`` are represented by ``struct``\s whose first member is a
``PyObject`` followed by any instance data needed.

In C, a pointer to a ``struct`` is equivalent to a pointer to its first member,
this makes it safe to cast from a type defined this way to and from
``PyObject*``.

Users rarely need to use the more specialized type when working with the CPython
API because most APIs expect and return plain :ref:`pyobjectptr` values.

One case where users often work with a concrete type is :ref:`py-type-object`
which is the result of :c:func:`Py_TYPE`.
