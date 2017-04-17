C Level Representation of Python Structures
===========================================

In the C API, Python objects are represented by a :ref:`pyobjectptr_`. A
:ref:`pyobjectptr_` is a reference to an object which is managed by the CPython
interpreter.

The ``PyObject`` struct is defined as:

.. code-block:: c

   typedef struct {
        Py_ssize_t ob_refcnt;   /* object reference count */
        PyTypeObject* ob_type;  /* object type */
   };


The ``ob_type`` field is a pointer to the Python type for the object. This is
how objects communicate their runtime type. For example: if a given
:ref:`pyobjectptr_` points to a unicode object, then the ``ob_type`` field will
be set to ``&PyUnicode_Type`` where ``PyUnicode_Type`` is the Python ``unicode``
type.

the ``ob_refcnt`` is the :ref:`ref-count` of the object. This is the number of
places where this object is being used.

Object Lifetime
---------------

CPython uses a reference counting garbage collection strategy. When an object is
created, its reference count starts at one, which is the reference owned by the
creator. The reference count can be increased with :ref:`py-incref` or decreased
with :ref:`py-decref`. As soon as the reference count reaches zero, the object
is no longer needed and it will be deallocated.

Type Specialization
-------------------

:ref:`pyobjectptr_` is an abstract reference that can point to any type;
however, we eventually need to actually store information on an object. The
normal way this is done is by defining a struct whose first member is a
``PyObject`` and then contains the rest of the data. Very rarely should the
type-specialized pointers be used instead of just :ref:`pyobjectptr_`.
