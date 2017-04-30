Writing a New Class in C
========================

So far we have discussed writing our own free functions, but, one of the nice
things about Python is that we can group related functions and data into a
class. The C API also allows us to write classes, though it is a lot of work.

.. warning::

   If you just need to optimize one or two methods, write *only* those methods
   in C as free functions. Writing a new type is a much more work and harder to
   maintain.

A type or class in Python holds the size in bytes to allocate for each new
instance and a virtual function table to resolve methods.

If we think back to the layout of a :c:type:`PyObject`, we have at least two
fields: :c:member:`PyObject.ob_type`, a pointer to the type for the instance,
and then :c:member:`PyObject.ob_refcnt`, the number of references to this
object. The instance knows how to find the implementation for all of the methods
by looking up its own type from the :c:member:`PyObject.ob_type` field and then
finding the given method in the type object.

For example: given the Python expression: ``repr(ob)``, we can start by
converting that into the equivalent C API call:
``PyObject_Repr(ob)``. :c:func:`PyObject_Repr` boils down into the following
call:

.. code-block:: c

   PyObject* result = Py_TYPE(ob)->tp_repr(ob);

Walking through this line, we start by getting a reference to the type of ``ob``
with :c:func:`Py_TYPE`. We then look up the :c:member:`PyTypeObject.tp_repr` slot
from the type of ``ob`` which is the implementation :func:`repr` for ``ob``.

There are slots for many of the builtin functions, for example:

- ``__getattr__``: :c:member:`PyTypeObject.tp_getattro`
- ``__setattr__``: :c:member:`PyTypeObject.tp_setattro`
- ``__call__``: :c:member:`PyTypeObject.tp_call`

and so on.

This works well for Python's special methods because it is a finite set of
functions that are called a lot, but types can define their own methods. For
example, given the class:

.. code-block:: python

   class PythonClass:
       def __repr__(self):
           return '<PythonClass instance>'

       def method(self):
           return 'something'

we know that there is a slot to hold ``__repr__``, but what about
``method``? This is not a special method that CPython uses so there is no
reserved slot in the :c:type:`PyTypeObject` struct. For these more free form
methods, types keep a Python dictionary object in the slot
:c:member:`PyTypeObject.tp_dict` from name to function object. Dispatching
through this dictionary is slower than going directly to the slot, but it is
required to allow these arbitrary names.

When defining methods on our type, we can lookup the values of these members on
``self`` just like we would in Python.

Instance Data
-------------

Using the type's slots or dictionary we can register methods for a given type,
but the data needs to be stored on the instance itself. When defining a new
class, we define the struct to represent an instance of the class. This struct
needs to start with a :c:type:`PyObject` field (note: not a pointer, a
:c:type:`PyObject` by *value*) which holds a pointer to our type and
:ref:`reference count <ref-count>`. Starting our struct with a
:c:type:`PyObject` makes it safe to cast to and from :c:type:`PyObject*` because
a pointer to a struct is equivalent to a pointer to its first member according
to the C standard.

After the :c:type:`PyObject` member, which is often called ``base``, we define
the instance data. Instance data can be a mix of Python values stored as
:c:type:`PyObject*` or regular C data.

For example, the following struct is for a class with a python :class:`list`
field, a :c:type:`Py_ssize_t` field, and a :c:type:`const char*` field.

.. code-block:: c

   typedef struct {
       PyObject base;  /* the base object values */
       PyObject* list_field;  /* a pointer to our list member */
       Py_ssize_t ssize_t_field;  /* our Py_ssize_t member */
       const char* string_field;  /* our const char* member */
   } myobject;

Slots Without a Python Equivalent
---------------------------------

While many slots have a 1:1 correspondence with a Python special method, there
are a few extra slots that we can control when writing a type in C.

Allocation and Deallocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Part of the definition of a class is how to allocate and deallocate
instances. This is controlled through the :c:member:`PyTypeObject.tp_alloc` and
:c:member:`PyTypeObject.tp_free` slots.

When we call :c:member:`PyTypeObject.tp_new` (``__new__``), we use
:c:member:`PyTypeObject.tp_alloc` to actually get the memory to store
``self``. In Python, we do this with a call to :meth:`object.__new__`; however,
in a C extension type we can pick our allocation strategy.

In almost all cases, we should use the standard Python allocator which is the
default value for :c:member:`PyTypeObject.tp_alloc` and
:c:member:`PyTypeObject.tp_free`. Only use a new allocator if that is core to
what you are trying to do.

Garbage Collection
~~~~~~~~~~~~~~~~~~

While CPython uses a reference counting system where objects are deallocated as
soon as their :c:member:`PyObject.ob_refcnt` hits 0, there are cases where a
cycle can form between objects which needs to be manually broken.

Classes implemented in C can hook into this machinery by implementing
:c:member:`PyTypeObject.tp_traverse` and :c:member:`PyTypeObject.tp_clear`
functions.

The :c:member:`PyTypeObject.tp_traverse` function allows the garbage collector
to touch all of the objects our instance has a reference to. This is used for
cycle detection.

The :c:member:`PyTypeObject.tp_clear` function allows the garbage collector to
break cycles by explicitly clearing all the references on a chosen object.

Flags
~~~~~

There is a special slot called :c:member:`PyTypeObject.tp_flags` which is a
bitmask of boolean values about a type. One of the bits is
:c:macro:`Py_TPFLAGS_HAVE_GC` which says that a type can hold references and
needs to participate in the cyclic garbage collector. Other flags like
:c:macro:`Py_TPFLAGS_LONG_SUBCLASS` mark that a class is a subclass of
:c:type:`PyLongObject`. This allows for faster isinstance checks against common
types.

When writing a new type, you should always start with
:c:macro:`Py_TPFLAGS_DEFAULT` and ``or`` in the other options.

Defining a :c:type:`PyTypeObject`
---------------------------------

Now that we understand a bit about the layout of types and instances, let's see
what a new type definition looks like. Below we define a class ``Queue`` with
two members: ``Py_ssize_t q_maxsize`` and ``PyObject* q_elements``. The
``q_maxsize`` is the maximum number of elements in the queue, and ``q_elements``
are the elements in the queue as Python :class:`list`.

.. code-block:: c

   typedef struct {
        PyObject q_base;       /* storage for our type and reference count */
        Py_ssize_t q_maxsize;  /* the maximum number of elements in q_elements */
        PyObject* q_elements;  /* the elements in the queue as a Python list */
    } queue;

    /* function implementations */

   static PyTypeObject queue_type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "queue.Queue",                              /* tp_name */
        sizeof(queue),                              /* tp_basicsize */
        0,                                          /* tp_itemsize */
        (destructor) queue_dealloc,                 /* tp_dealloc */
        0,                                          /* tp_print */
        0,                                          /* tp_getattr */
        0,                                          /* tp_setattr */
        0,                                          /* tp_reserved */
        (reprfunc) queue_repr,                      /* tp_repr */
        0,                                          /* tp_as_number */
        0,                                          /* tp_as_sequence */
        0,                                          /* tp_as_mapping */
        0,                                          /* tp_hash */
        0,                                          /* tp_call */
        0,                                          /* tp_str */
        0,                                          /* tp_getattro */
        0,                                          /* tp_setattro */
        0,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_HAVE_GC,                         /* tp_flags */
        queue_doc,                                  /* tp_doc */
        (traverseproc) queue_traverse,              /* tp_traverse */
        (inquiry) queue_clear,                      /* tp_clear */
        0,                                          /* tp_richcompare */
        0,                                          /* tp_weaklistoffset */
        0,                                          /* tp_iter */
        0,                                          /* tp_iternext */
        queue_methods,                              /* tp_methods */
        0,                                          /* tp_members */
        0,                                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,                                          /* tp_dictoffset */
        0,                                          /* tp_init */
        0,                                          /* tp_alloc */
        (newfunc) queue_new,                        /* tp_new */
   };

This is the definition for a class called ``Queue`` from python an ``queue``
from C. Instances are defined by the ``queue`` struct at the top. The type
implements only a subset of the possible functions.

The :c:member:`PyTypeObject.tp_basicsize` is set to be the size of out ``queue``
struct. This means that instances of a ``Queue`` are of size ``queue``.

The :c:member:`PyTypeObject.tp_itemsize` is set to 0 because all instances of
``queue`` use the same amount of space. This field is designed to implement
compact collections like :class:`tuple`.

The :c:member:`PyTypeObject.tp_dealloc` is set to a deallocation function. This
function needs to release the ``queue``\'s reference to ``q_elements``.

The :c:member:`PyTypeObject.tp_flags` is set to :c:macro:`Py_TPFLAGS_DEFAULT` |
:c:macro:`Py_TPFLAGS_HAVE_GC`. This means we have both the default flags and the
HAVE_GC bit. Because the :c:macro:`Py_TPFLAGS_HAVE_GC` bit is set, we also have
a :c:member:`PyTypeObject.tp_traverse` and :c:member:`PyTypeObject.tp_clear`
function.

This type also has some custom methods so we need to pass an array to
:c:member:`PyTypeObject.tp_methods`.

Finally, we need to construct our new objects so we have a
:c:member:`PyTypeObject.tp_new` function set.

.. note::

   If :c:member:`PyTypeObject.tp_new` is :c:data:`NULL`, you cannot create new
   instances of the class.

There are many more slots that we are not using because we don't need them. You
can read about what the slots are for in the docs for :c:type:`PyTypeObject`.

Readying a Type
---------------

Our ``queue_type`` struct has a lot of :c:data:`NULL` members because we want to
inherit their value from our base class, which in this case is
:class:`object`. Looking back at the code in :c:func:`PyObject_Repr` above, you
can see that we are not doing a :c:data:`NULL` check or class traversal, we just
get the :c:member:`PyTypeObject.tp_repr` and call it. In order to avoid these
checks Python has a function called :c:func:`PyType_Ready` which copies the
slots down from the base class. This function also copies any methods that are
stored in the :c:member:`PyTypeObject.tp_dict`.

To ready a type, call :c:func:`PyType_Ready` in the :c:macro:`PyMODINIT_FUNC`
for the module defining the class.

Implement ``Queue.push`` and ``Queue.pop``
------------------------------------------

As an exercise, implement for ``Queue.push`` and ``Queue.pop``. Try to
implement these functions using the :c:type:`PyListObject` API functions.

Use :c:func:`PyArg_ParseTupleAndKeywords` to accept arguments for
``Queue.push``. ``Queue.pop`` should use :c:macro:`METH_NOARGS` and just accept
``self``.

Remember to check for exceptions after calling API functions.
