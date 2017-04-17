Appendix
========

Glossary
--------

.. _new-reference:

New Reference
~~~~~~~~~~~~~

A new reference is a pyobjectptr_ which must be managed by the user. The user
must ensure that py-decref_ is called when the object is no longer needed.

.. _borrowed-reference:

Borrowed Reference
~~~~~~~~~~~~~~~~~~

A borrowed reference is a pyobjectptr_ which is not owned. Users should *not*
call py-decref_ when they are done with this object. A borrowed reference can
become a new-reference_ by calling py-incref_ on the it.

Reference Counting
------------------

.. _ref-count:

Reference Count
~~~~~~~~~~~~~~~

The number of refererants of a given object. This is the count of objects which
refer to the given object.

When an object is created, its reference count starts at one, which is the
reference owned by the creator. The reference count can be increased with
py-incref_ or decreased with py-decref_. As soon as the reference count reaches
zero, the object is no longer needed and it will be deallocated.

.. _py-incref:


``Py_INCREF``
~~~~~~~~~~~~~

The ``Py_INCREF`` macro increments the ref-count_ of an object by one.

.. _py-decref:


``Py_DECREF``
~~~~~~~~~~~~~

The ``Py_DECREF`` macro decrements the ref-count_ of an object by one.

.. _py-xdecref:

``Py_XDECREF``
~~~~~~~~~~~~~~

The ``Py_XDECREF`` macro decrements the ref-count_ of an object after performing
a ``NULL`` check.

.. _py-clear:

``Py_CLEAR``
~~~~~~~~~~~~

The ``Py_CLEAR`` macro decrements the ref-count_ of an object and then sets the
input pyobjectptr_ to ``NULL``.


CPython Structures
------------------

.. _py-ssize-t:

``Py_ssize_t``
~~~~~~~~~~~~~~

A ``Py_ssize_`` is a signed integral type used to hold counts and object sizes.

On many platforms this is an alias of ``ssize_t``.

.. _pyobjectptr:

``PyObject*``
~~~~~~~~~~~~~

https://docs.python.org/3.6/c-api/structures.html#c.PyObject

``PyObject`` is the structure which holds Python values. The definition looks
like:

.. code-block:: c

   typedef struct {
        Py_ssize_t ob_refcnt;   /* object reference count */
        PyTypeObject* ob_type;  /* object type */
   };


.. _py-type-object:


``PyTypeObject*``
~~~~~~~~~~~~~~~~~

https://docs.python.org/3/c-api/type.html

``PyTypeObject`` is the structure which holds Python ``type`` objects.

Casting Rules
`````````````

A ``PyTypeObject*`` can safely be cast to a :ref:`pyobjectptr`.

:ref:`pyobjectptr`\s can be cast to ``PyTypeObject*``\s only after a
``PyType_Check``.

.. _py-long-object:

``PyLongObject*``
~~~~~~~~~~~~~~~~~

https://docs.python.org/3/c-api/long.html

``PyLongObject`` is the structure which holds Python ``int`` objects. This is
called a ``PyLongObject`` as a hold over from when arbitrary width integer was
called a ``long`` object in Python 2.

Casting Rules
`````````````

A ``PyLongObject*`` can safely be cast to a :ref:`pyobjectptr`.

:ref:`pyobjectptr`\s can be cast to ``PyLongObject*``\s only after a
``PyLong_Check``.

CPython Functions
-----------------

.. _py-long-as-unsigned-long:

``PyLong_AsUnsignedLong``
~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: unsigned long PyLong_AsUnsignedLong(PyObject* ob)

   Convert a ``PyObject*`` of type :ref:`py-long-object` to an ``unsigned
   long``. If ``ob`` is not a ``long`` object, an exception is raised.

   :param PyObject* ob: The object to convert.
   :return: ``ob`` as an ``unsigned long``.

``PyLong_FromUnsignedLong``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: PyObject* PyLong_FromUnsignedLong(unsigned long l)

   Convert an ``unsigned long`` into a :ref:`pyobjectptr`. If the object cannot
   be allocated an exception is raised.

   :param l: The unsigned long to convert to a pyobjectptr.
   :return: a :ref:`new reference <new-reference>` to ``l`` as a Python
            object.