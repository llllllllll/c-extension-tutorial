Appendix
========

Reference Counting
------------------

.. _ref-count:

Reference Count
~~~~~~~~~~~~~~~

The number of references to a given object. This is the count of objects which
refer to the given object.

When an object is created, its reference count starts at one, which is the
reference owned by the creator. The reference count can be increased with
:c:func:`Py_INCREF` or decreased with :c:func:`Py_DECREF`. As soon as the
reference count reaches zero, the object is no longer needed and it will be
deallocated.

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
**not** call :c:func:`Py_DECREF` when they are done with this object. A borrowed
reference can become a :ref:`new reference <new-reference>` by calling
:c:func:`Py_INCREF` on the it.

.. _reference-stealing:

Reference Stealing
~~~~~~~~~~~~~~~~~~

Some functions will "steal" a reference to an argument. This means the function
will assume ownership of the reference that has been given to it. Callers should
**not** call :c:func:`Py_DECREF` the object after the reference has been
stolen. If you need to keep your own reference to the object after it has been
stolen you need to call :c:func:`Py_INCREF` **before** the reference is stolen.

An example of this pattern is :c:func:`PyList_SetItem`. This function steals a
reference to the ``value`` argument. This is to make it easier to allocate new
objects and put them right into a :c:type:`PyListObject` without needing extra
calls to :c:func:`Py_INCREF`.

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
   an object if ``ob`` is not :c:data:`NULL`.

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
   an object if ``ob`` is not :c:data:`NULL`.

   :param PyObject* ob: The object to decrement the reference count of.

``Py_CLEAR``
~~~~~~~~~~~~

.. c:function:: void Py_CLEAR(PyObject* ob)

   The ``Py_CLEAR`` macro decrements the :ref:`reference count <ref-count>` of
   an object and then sets the input :c:type:`PyObject*` to :c:data:`NULL`.

   :param PyObject* ob: The object to decrement the reference count of and set
                        to :c:data:`NULL`.

CPython Types
-------------

``Py_ssize_t``
~~~~~~~~~~~~~~

.. c:type:: Py_ssize_t

   A ``Py_ssize_`` is a signed integral type used to hold counts and object
   sizes. On many platforms this is an alias of :c:type:`ssize_t`.

``PyObject``
~~~~~~~~~~~~

.. c:type:: PyObject

   https://docs.python.org/3.6/c-api/structures.html#c.PyObject

   ``PyObject`` is the structure which holds Python values. The definition looks
   like:

   .. c:member:: Py_ssize_t ob_refcnt

      The object's :ref:`reference count <ref-count>`.

   .. c:member:: PyTypeObject* ob_type

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

Fields
``````

.. c:member:: const char* PyTypeObject.tp_name

   The fully qualified name to the class as a C string. This string must stay
   alive forever.

   This field is not inherited.

.. c:member:: Py_ssize_t PyTypeObject.tp_basicsize

   The base size of instances of this class. You should use ``sizeof`` to derive
   this from the instance struct type.

   This field is inherited

.. c:member:: Py_ssize_t PyTypeObject.tp_itemsize

   Some classes require variable storage to hold instances. For example,
   :c:type:`PyTupleObject`\s stores all of the members inline. The
   :c:member:`PyTypeObject.tp_basicsize` of a tuple would be the base object and
   element count, the :c:member:`PyTypeObject.tp_itemsize` would be
   ``sizeof(PyObject*)``. If instances are always the same size, this field
   should be 0.

.. c:member:: destructor PyTypeObject.tp_dealloc

   A pointer to the object destructor. This function is called when the
   :ref:`reference count <ref-count>` hits 0. This function should release
   any references that it owns and then defer to
   :c:member:`PyTypeObject.tp_free` to deallocate the memory.

   This field is inherited.

.. c:member:: printfunc PyTypeObject.tp_print

   Reserved slot from Python 2. This is now unused.

.. c:member:: getattrfunc PyTypeObject.tp_getattr

   Deprecated in favor of :c:member:`PyTypeObject.tp_getattro`

.. c:member:: setattrfunc PyTypeObject.tp_setattr

   Deprecated in favor of :c:member:`PyTypeObject.tp_setattro`

.. c:member:: PyAsyncMethods* PyTypeObject.tp_as_async

   Pointer to additional functions used when an object supports the Async
   Protocol. If the type does not support the Async Protocol, this can be
   :c:data:`NULL`.

   The value of the pointer is not inherited, but the members are inherited
   individually.

.. c:member:: reprfunc PyTypeObject.tp_repr

   C API equivalent of ``__repr__``

   This can be invoked with :c:func:`PyObject_Repr`.

   This field is inherited.

.. c:member:: PyNumberMethods* PyTypeObject.tp_as_number

   Pointer to additional functions used when an object supports the :ref:`number
   Protocol <number-api>`. If the type does not support the Number Protocol,
   this can be :c:data:`NULL`.

   The value of the pointer is not inherited, but the members are inherited
   individually.

   .. c:type:: PyNumberMethods

       A collection of function pointers used to support the :ref:`number-api`.

       .. code-block:: c

          typedef struct {
              binaryfunc nb_add;
              binaryfunc nb_subtract;
              binaryfunc nb_multiply;
              binaryfunc nb_remainder;
              binaryfunc nb_divmod;
              ternaryfunc nb_power;
              unaryfunc nb_negative;
              unaryfunc nb_positive;
              unaryfunc nb_absolute;
              inquiry nb_bool;
              unaryfunc nb_invert;
              binaryfunc nb_lshift;
              binaryfunc nb_rshift;
              binaryfunc nb_and;
              binaryfunc nb_xor;
              binaryfunc nb_or;
              unaryfunc nb_int;
              void *nb_reserved;
              unaryfunc nb_float;

              binaryfunc nb_inplace_add;
              binaryfunc nb_inplace_subtract;
              binaryfunc nb_inplace_multiply;
              binaryfunc nb_inplace_remainder;
              ternaryfunc nb_inplace_power;
              binaryfunc nb_inplace_lshift;
              binaryfunc nb_inplace_rshift;
              binaryfunc nb_inplace_and;
              binaryfunc nb_inplace_xor;
              binaryfunc nb_inplace_or;

              binaryfunc nb_floor_divide;
              binaryfunc nb_true_divide;
              binaryfunc nb_inplace_floor_divide;
              binaryfunc nb_inplace_true_divide;

              unaryfunc nb_index;

              binaryfunc nb_matrix_multiply;
              binaryfunc nb_inplace_matrix_multiply;
        } PyNumberMethods;


.. c:member:: PySequenceMethods* PyTypeObject.tp_as_sequence

   Pointer to additional functions used when an object supports the Sequence
   Protocol. If the type does not support the Sequence Protocol, this can be
   :c:data:`NULL`.

   The value of the pointer is not inherited, but the members are inherited
   individually.

   .. c:type:: PySequenceMethods

      A collection of function pointers used to support the Sequence API.

      .. c:member:: lenfunc sq_length

         The function used to support :c:func:`PyObject_Size`. This function
         will automatically be converted into a Python ``__len__``.

      .. c:member:: binaryfunc sq_concat

         The function used to support :c:func:`PySequence_Concat`. If there is
         no :c:member:`PyNumberMethods.nb_add` function, this will be turned
         into a ``__add__`` function (like ``list + list`` in Python).

      .. c:member:: ssizeargfunc sq_repeat

         The function used to support :c:func:`PySequence_Repeat`. If there is
         no :c:member:`PyNumberMethods.nb_multiply` this will be turned into a
         ``__mul__`` function (like ``list * int`` in Python).

      .. c:member:: ssizeargfunc sq_item

         The function used to support :c:func:`PySequenceMethods.sq_item`. This
         function will be converted into a ``__getitem__`` method in Python.

      .. c:member:: ssizeobjargproc sq_ass_item

         The function used to support :c:func:`PySequence_SetItem`. This slot
         can be :c:data:`NULL` if the object doesn't support assignment (like
         ``tuple``).  This will be converted into a ``__setitem__`` method in
         Python.

      .. c:member:: objobjproc sq_contains

         The function used to support :c:func:`PySequence_Contains`. If this is
         left :c:data:`NULL`, a linear search will be performed. This function
         will be converted into a ``__contains__`` method in Python.

      .. c:member:: binaryfunc sq_inplace_concat

         The function used to support :c:func:`PySequence_InPlaceConcat`. It
         should modify ``self`` in place and then return ``self``. This function
         is like ``list += list`` in Python.

      .. c:member:: binaryfunc sq_inplace_repeat

         The function used to support :c:func:`PySequence_InPlaceRepeat`. It
         should modify ``self`` in place and then return ``self``. This function
         is like ``list *= int`` in Python.

.. c:member:: PyMappingMethods* PyTypeObject.tp_as_mapping

   Pointer to additional functions used when an object supports the Mapping
   Protocol. If the type does not support the Mapping Protocol, this can be
   :c:data:`NULL`.

   The value of the pointer is not inherited, but the members are inherited
   individually.

.. c:member:: hashfunc PyTypeObject.tp_hash

   C API equivalent of ``__hash__``.

   This can be set to :c:func:`PyObject_HashNotImplemented` which is the
   equivalent of adding ``__hash__ = None`` in Python.

   This can be invoked with :c:func:`PyObject_Hash`.

   This field is inherited along with :c:member:`PyTypeObject.tp_richcompare`
   only when the subtype does not override either function.

.. c:member:: ternaryfunc PyTypeObject.tp_call

   C API equivalent of ``__call__``.

   This field is inherited.

.. c:member:: reprfunc PyTypeObject.tp_str

   The C API equivalent of ``_str__``. If this not set,
   :c:member:`PyTypeObject.tp_repr` will be used.

   This can be invoked with :c:func:`PyObject_Str`.

   This field is inherited.

.. c:member:: getattrofunc PyTypeObject.tp_getattro

   The C API equivalent of ``__getattr__``. This can be set to
   :c:func:`PyObject_GenericGetAttr` which implements standard attribute lookup.

   This can be invoked with :c:func:`PyObject_GetAttr`.

   This field is inherited.

   .. note::

      To use :c:func:`PyObject_GenericGetAttr` the
      :c:member:`PyTypeObject.tp_dictoffset` must be nonzero.

.. c:member:: setattrofunc PyTypeObject.tp_setattrofunc

   The C API equivalent of ``__setattr__``. This can be set to
   :c:func:`PyObject_GenericSetAttr` which implements standard attribute
   assignment.

   This can be invoked with :c:func:`PyObject_SetAttr`.

   This field is inherited.

   .. note::

      To use :c:func:`PyObject_GenericSetAttr` the
      :c:member:`PyTypeObject.tp_dictoffset` must be nonzero.

.. c:member:: PyBufferProcs* PyTypeObject.tp_as_buffer

   Pointer to additional functions used when an object supports the Buffer
   Protocol. If the type does not support the Buffer Protocol, this can be
   :c:data:`NULL`.

   The value of the pointer is not inherited, but the members are inherited
   individually.

.. c:member:: unsigned long PyTypeObject.tp_flags

   A bitmask of information about the type.

   .. c:macro:: Py_TPFLAGS_HEAPTYPE

      This bit is set when the type object is allocated on the heap. This is
      true for classes defined in Python.

   .. c:macro:: Py_TPFLAGS_BASETYPE

      This bit is set when the type can be subclassed. Sometimes we don't want
      to treat out members as virtual so we ignore the possibility of subclasses
      changing methods.

      Attempting to subclass for a type without this bit set will generate an
      error like:

      .. code-block:: python

         TypeError: type '<name>' is not an acceptable base type

   .. c:macro:: Py_TPFLAGS_READY

      Marks that :c:func:`PyType_Ready` has been called. You should not
      explicitly set this flag.

   .. c:macro:: Py_TPFLAGS_READYING

      Marks that :c:func:`PyType_Ready` is in the process of readying the
      type. You should not explicitly set this flag.

   .. c:macro:: Py_TPFLAGS_HAVE_GC

      Marks that this type supports the cyclic garbage collector. If this is
      set, instances need to be allocated with :c:func:`PyObject_GC_New` and
      freed with :c:func:`PyObject_GC_Del`. If this bit is set the
      :c:member:`PyTypeObject.tp_traverse` function must be set.

   .. c:macro:: Py_TPFLAGS_DEFAULT

      The default set of bits that should be set for all new classes.

   .. c:macro:: Py_TPFLAGS_LONG_SUBCLASS

   .. c:macro:: Py_TPFLAGS_LIST_SUBCLASS

   .. c:macro:: Py_TPFLAGS_TUPLE_SUBCLASS

   .. c:macro:: Py_TPFLAGS_BYTES_SUBCLASS

   .. c:macro:: Py_TPFLAGS_UNICODE_SUBCLASS

   .. c:macro:: Py_TPFLAGS_DICT_SUBCLASS

   .. c:macro:: Py_TPFLAGS_BASE_EXC_SUBCLASS

   .. c:macro:: Py_TPFLAGS_TYPE_SUBCLASS

      Marks that the type is a subclass of one of the builtin types. These
      dramatically speed up common ``Py*_Check`` calls which can then use a bit
      and instead of a generic :c:func:`PyObject_Isinstance` call. You should
      not set these bits directly, :c:func:`PyType_Ready` will do this if
      needed.

   .. c:macro:: Py_TPFLAGS_HAVE_FINALIZE

      Marks that the :c:member:`PyTypeObject.tp_finalize` should be called on
      object destruction.

.. c:member:: const char* PyTypeObject.tp_doc

   A pointer to the docstring as a C string. If :c:data:`NULL`, ``__doc__`` will
   be ``None``.

   This field is not inherited.

.. c:member:: traverseproc PyTypeObject.tp_traverse

   A pointer to the cyclical garbage collector traversal function. This is only
   called if :c:macro:`Py_TPFLAGS_HAVE_GC` is set.

   This function needs to call :c:func:`Py_VISIT` on all of the members which
   may participate in a cycle.

   To use the :c:func:`Py_VISIT` macro the :c:type:`visitproc` argument *must*
   be called ``visit`` and the :c:type:`void*` argument must be called ``arg``.

   Below we have a type with two :c:type:`PyObject*` fields and one non-object
   field.

   .. code-block:: c

      typedef struct {
          PyObject mt_base;
          PyObject* mt_object_member;
          PyObject* mt_other_object_member;
          Py_ssize_t mt_not_an_object;
      } mytype;

      static int
      mytype_traverse(mytype* self, visitproc visit, void* arg)
      {
          Py_VISIT(self->mt_object_member);
          Py_VISIT(self->mt_other_object_member);
          /* note: not visiting self->mt_not_an_object because it is not a
             PyObject* */
          return 0;
      }

   .. note::

      It is safe to not traverse immutable scalar members, for example:
      :c:type:`PyLongObject*` members. It doesn't hurt to visit all
      :c:type:`PyObject*` members so if you aren't sure, just visit it!

.. c:member:: inquiry PyTypeObject.tp_clear

   Clear any references owned by this object **while keeping the object in a
   valid state**. Valid state means that the interpreter should not segfault
   because an object assumes members are non-null and then ``tp_clear`` sets
   them to :c:data:`NULL`. An example of this idea is :meth:`list.clear`. This
   drops all of the references owned by the list; however, at the end you still
   have a valid list. This function is only called if
   :c:macro:`Py_TPFLAGS_HAVE_GC` is set.

   When clearing references the :c:func:`Py_CLEAR` macro should be used instead
   of just :c:func:`Py_DECREF`. This is because decref can trigger a
   deallocation which can invoke arbitrary Python code through an object's
   ``__del__`` method. This code can reference back to the object being cleared
   and we don't want to return a pointer to the recently destroyed object.

   Below we have a type with two :c:type:`PyObject*` fields and one non-object
   field.

   .. code-block:: c

      typedef struct {
          PyObject mt_base;
          PyObject* mt_object_member;
          PyObject* mt_other_object_member;
          Py_ssize_t mt_not_an_object;
      } mytype;

      static int
      mytype_clear(mytype* self)
      {
          Py_CLEAR(self->mt_object_member);
          Py_CLEAR(self->mt_other_object_member);
          /* note: not clearing self->mt_not_an_object because it is not a
             PyObject* */
          return 0;
      }

   .. note::

      Immutable objects can skip defining a :c:member:`PyTypeObject.tp_clear`
      field. It is impossible to form a reference cycle with all immutable
      objects which means the other object's :c:member:`PyTypeObject.tp_clear`
      functions must be sufficient to clear the cycle.

.. c:member:: richcmpfunc PyTypeObject.tp_richcompare

   The function which supports :c:func:`PyObject_Richcompare` for this type.

   See :c:func:`PyObject_Richcompare` for more information about how this
   function works.

   This field is inherited along with :c:member:`PyTypeObject.tp_hash` only when
   the subtype does not override either function.

.. c:member:: Py_ssize_t PyTypeObject.tp_weaklistoffset

   The offset into an object structure where the weaklist is stored. This is
   used for making objects weakly referenceable. If an object is not weakly
   referenceable, this field should be set to 0. This offset needs to point to a
   :c:data:`NULL` initialized :c:type:`PyObject*` slot in the instance
   structure.

   In Python, if a class defines a ``__weakref__`` slot in ``__slots__``, that
   offset will be used as the :c:member:`PyTypeObject.tp_weaklistoffset`.

   Below we have a weakly referenceable type:

   .. code-block:: c
      :emphasize-lines: 3, 9

      typedef struct {
          PyObject wr_base;
          PyObject* wr_weaklist;
          /* other data if we want */
      } mytype;

      PyTypeObject mytype_type = {
          /* ... */
          offsetof(mytype, wr_weaklist),  /* tp_weaklistoffset */
          /* ... */
      };

   Note that we use the ``ofsetoff`` operator to compute the offset accounting
   for the size of all members before ``wr_weaklist`` and any padding added by
   the compiler.

   This field is inherited.

   .. note::

      Do not confuse this field with :c:member:`PyTypeObject.tp_weaklist` which
      is the weaklist storage for taking weak references of the type object
      itself.

.. c:member:: getiterfunc PyTypeObject.tp_iter

   The C API equivalent of ``__iter__``.

   This can be invoked with :c:func:`PyObject_Iter`.

   This field is inherited.

   .. note::

      Objects may still be iterable without this function set if the support the
      Sequence Protocol. The :c:func:`PySequenceMethods.sq_item` function will
      be used from 0 until an :c:data:`PyExc_IndexError` is raised just like in
      Python with ``__getitem__``.

.. c:member:: iternextfunc PyTypeObject.tp_iternext

   The C API equivalent of ``__next__``. The core difference is that
   :c:data:`PyExc_StopIteration` does **not** need to be set when the iterator
   is exhausted. This will be done before returning to Python but the exception
   overhead can be avoided when being called from C.

   If this slot is set, :c:member:`PyTypeObject.tp_iter` should be a function
   which returns a :ref:`new reference <new-reference>` to ``self``.

   This can be invoked with :c:func:`PyIter_Next`.

.. c:member:: PyMethodDef* PyTypeObject.tp_methods

   A :c:data:`NULL` terminated array of :c:type:`PyMethodDef` structures which
   will become the methods of the class. For each :c:type:`PyMethodDef` in this
   list, a function object will be created and stored in the
   :c:member:`PyTypeObject.tp_dict`.

   The value of this field is not inherited but the methods will be.

.. c:member:: PyMemberDef* PyTypeObject.tp_members

   A :c:data:`NULL` terminated array of :c:type:`PyMemberDef` structures which
   will become the methods of the class. For each :c:type:`PyMemberDef` in this
   list, a descriptor object will be created and stored in the
   :c:member:`PyTypeObject.tp_dict`.

   The value of this field is not inherited but the members will be.

   .. c:type:: PyMemberDef

      A structure which defines a descriptor for exposing a C member as a Python
      member.

      .. c:member:: char* name

         The name of the member as a C string.

      .. c:member:: int type

         The type code of the member.

         .. c:macro:: T_SHORT

            A :c:type:`short` member to be converted into a Python ``int``.

         .. c:macro:: T_INT

            A :c:type:`int` member to be converted into a Python ``int``.

         .. c:macro:: T_LONG

            A :c:type:`long` member to be converted into a Python ``int``.

         .. c:macro:: T_FLOAT

            A :c:type:`float` member to be converted into a Python ``float``.

         .. c:macro:: T_DOUBLE

            A :c:type:`double` member to be converted into a Python ``float``.

         .. c:macro:: T_STRING

            A :c:type:`char*` member to be converted into a Python ``str``.

            .. note::

               This implies :c:macro:`READONLY`.

         .. c:macro:: T_OBJECT

            Get a :c:type:`PyObject*`. If the pointer is :c:data:`NULL`,
            ``None`` will be returned.

            :c:macro:`T_OBJECT_EX` is often a better choice because of how it
            handles ``del``.

         .. c:macro:: T_OBJECT_EX

            Get a :c:type:`PyObject*`. If the pointer is :c:data:`NULL`,
            a :c:data:`PyExc_AttributeError` will be raised.

         .. c:macro:: T_CHAR

            A :c:type:`char` member to be converted into a Python ``str`` of
            length 1.

         .. c:macro:: T_BYTE

            A :c:type:`char` member to be converted into a Python ``int``.

         .. c:macro:: T_UBYTE

            A :c:type:`unsigned char` member to be converted into a Python
            ``int``.

         .. c:macro:: T_UINT

            A :c:type:`unsigned int` member to be converted into a Python
            ``int``.

         .. c:macro:: T_USHORT

            A :c:type:`unsigned short` member to be converted into a Python
            ``int``.

         .. c:macro:: T_ULONG

            A :c:type:`unsigned short` member to be converted into a Python
            ``int``.

         .. c:macro:: T_BOOL

            A :c:type:`char` member to be converted into a Python ``bool``.

         .. c:macro:: T_LONGLONG

            A :c:type:`long long` member to be converted into a Python ``int``.

         .. c:macro:: T_ULONGLONG

            A :c:type:`unsigned long long` member to be converted into a Python
            ``int``.

         .. c:macro:: T_PYSSIZET

            A :c:type:`Py_ssize_t` member to be converted into a Python ``int``.

      .. c:member:: Py_ssize_t offset

         The offset into the C structure where this member appears. You should
         always compute this offset with the ``offsetof`` operator.

      .. c:member:: int flags

         Flag bits for indicating read or write status. The options are ``0``
         for read and write access or :c:macro:`READONLY` for read only access.

         :c:macro:`T_STRING` forces :c:macro:`READONLY`.

         Only :c:macro:`T_OBJECT` and :c:macro:`T_OBJECT_EX` can be deleted with
         ``del`` which sets the pointer to :c:data:`NULL`.

      .. c:member:: char* PyMemberDef.doc

         The docstring for the member. If set to :c:data:`NULL` ``__doc__`` will
         be ``None``.

.. c:member:: PyGetSetDef* PyTypeObject.tp_getset

   A :c:data:`NULL` terminated array of :c:type:`PyGetSetDef` structures which
   will become the methods of the class. For each :c:type:`PyGetSetDef` in this
   list, a descriptor object will be created and stored in the
   :c:member:`PyTypeObject.tp_dict`..

   .. c:type:: PyGetSetDef

      A :c:type:`PyGetSetDef` defines a computed attribute like a Python
      :class:`property`

      .. c:member:: char* name

         The name of this attribute as a C string.

      .. c:member:: getter get

         The function used to compute this attribute.

      .. c:member:: setter set

         The function used to assign to this attribute. If the attribute is
         readonly, this may be :c:data:`NULL`.

      .. c:member:: char* doc

         The docstring as C string. If this is :c:data:`NULL`, ``__doc__`` will
         be ``None``.

      .. c:member:: void* closure

         Extra data to be passed to the getter and setter functions.

      .. c:type:: getter

         .. code-block:: c

            typedef PyObject* (*getter)(PyObject* self, void* closure);

      .. c:type:: setter

         .. code-block:: c

            typedef int (*setter)(PyObject* self,
                                  PyObject* value,
                                  void* closure);

.. c:member:: PyTypeObject* PyTypeObject.tp_base

   The base class for this new type. If set to :c:data:`NULL`,
   ``&PyBaseObject_Type`` (which is ``object`` in Python) is used.

   This field is not inherited.

.. c:member:: PyObject* PyTypeObject.tp_dict

   The type's dictionary (``__dict__``). This is initialized by
   :c:func:`PyType_Ready`.

   This field is not inherited but the members of the dictionary are.

   .. note::

      Types sometimes use what is called a "split keys dictionary" instead of a
      normal Python dict. You should not use the ``PyDict_*`` functions to
      modify this object.

.. c:member:: descrgetfunc PyTypeObject.tp_descr_get

   The C API equivalent of ``__get__`` for the descriptor protocol.

   This field is inherited.

   .. c:type:: descrgetfunc

      .. code-block:: c

         typedef PyObject* (*descrgetfunc)(PyObject* self,
                                           PyObject* instance,
                                           PyObject* owner);

.. c:member:: descrsetfunc PyTypeObject.tp_descr_set

   The C API equivalent of ``__set__`` and ``__delete__`` for the descriptor
   protocol.

   This field is inherited.

   .. c:type:: descrsetfunc

      .. code-block:: c

         typedef PyObject* (*descrsetfunc)(PyObject* self,
                                           PyObject* instance,
                                           PyObject* value);

   If ``value`` is :c:data:`NULL`, this should delete the value.

.. c:member:: Py_ssize_t PyTypeObject.tp_dictoffset

   The offset into an object structure where the ``__dict__`` is stored. This is
   used for allowing objects to have arbitrary attributes like a normal Python
   object.

   For some objects, we only want to support a specific set of fields stored in
   the instance struct so we can set :c:member:`PyTypeObject.tp_dictoffset`
   to 0 which indicates that we do not have a ``__dict__``.

   The instance structure must have a :c:data:`NULL` initialized
   :c:type:`PyObject*` member at the offset if it is nonzero.

   Below we have a type with a ``__dict__``:

   .. code-block:: c
      :emphasize-lines: 3, 9

      typedef struct {
          PyObject wd_base;
          PyObject* wd_dict;
          /* other data if we want */
      } mytype;

      PyTypeObject mytype_type = {
          /* ... */
          offsetof(mytype, wd_dict),  /* tp_dictoffset */
          /* ... */
      };

   Note that we use the ``ofsetoff`` operator to compute the offset
   accounting for the size of all members before ``wd_dict`` and any padding
   added by the compiler.

   This field is inherited.

   .. warning::

      It is not common to define a new class in C that uses a nonzero
      :c:member:`PyTypeObject.tp_dictoffset`. You are going to lose a lot of
      potential performance gains by dispatching through a dictionary for all
      lookups.

.. c:member:: initproc PyTypeObject.tp_init

   The C API equivalent of ``__init__``. This is only used for mutable types.

   This field is inherited.

.. c:member:: allocfunc PyTypeObject.tp_alloc

   A function used to allocate memory for the new instance. This is different
   from ``__new__`` in that it does not initialize any member data. It exists
   only as a way to separate object allocation and initialization.

   The :c:type:`Py_ssize_t` argument is the number of items in this
   instance. This is only meaningful for types with nonzero
   :c:member:`PyTypeObject.tp_itemsize`.

   All :c:macro:`Py_TPFLAGS_HEAPTYPE` objects use :c:func:`PyType_GenericAlloc`
   to force standard heap allocations. This is the default and recommended value
   for all types.

   This field is inherited by static (C defined) subtypes but not heap (Python
   defined) subtypes.

.. c:member:: newfunc PyTypeObject.tp_new

   A function used to allocate and initialize new instances of the type.

   :c:member:`PyTypeObject.tp_new` should use :c:member:`PyTypeObject.tp_alloc`
   to allocate the raw memory for the new instance.

   If the type is immutable, the rest of the initialization should happen in the
   :c:member:`PyTypeObject.tp_new`. If the type is mutable, the initialization
   should happen in the :c:member:`PyTypeObject.tp_init` which will be called
   automatically.

   This field is inherited.

.. c:member:: destructor PyTypeObject.tp_free

   A function used to deallocate the memory allocated with
   :c:member:`PyTypeObject.tp_alloc`.

   By default this is a function that is compatible with the
   :c:func:`PyType_GenericAlloc` allocator.

   This field is inherited by static (C defined) subtypes but not heap (Python
   defined) subtypes.

.. c:member:: inquiry PyTypeObject.tp_is_gc

   This function is used when a type has a mix of statically and dynamically
   allocated instances. In this case, the :c:macro:`Py_TPFLAGS_HAVE_GC` flag is
   not enough to know if an instance can be collected. In this case, the
   function should be implemented to return True if the instance can be
   collected, otherwise False.

   This field is inherited.

   .. note::

      This is a very uncommon function to implement. It is mainly used to
      support static and dynamic class objects in CPython itself.

.. c:member:: PyObject* PyTypeObject.tp_bases

   A tuple of the base types. This field is set by :c:func:`PyType_Ready` and
   should not be manually modified.

   This field is not inherited.

.. c:member:: PyObject* PyTypeObject.tp_mro

   A tuple of the full method resolution order. This field is set by
   :c:func:`PyType_Ready` and should not be manually modified.

   This field is not inherited.

.. c:member:: destructor PyTypeObject.tp_finalize

   A function called before the garbage collector clears references or
   deallocates the object. This function is only called if
   :c:macro:`Py_TPFLAGS_HAVE_FINALIZE` is set.

   This can be useful for releasing C level resources like file descriptors.

   See :pep:`442` for more information about this field.

   This field is inherited.

.. c:member:: PyObject* PyTypeObject.tp_cache

   Internal use only.

.. c:member:: PyObject* PyTypeObject.tp_subclasses.

   Internal use only.

.. c:member:: PyObject* PyTypeObject.tp_weaklist

   The member to hold the weaklist for weak references to the type object
   itself.

   .. note::

      Do not confuse this field with :c:member:`PyTypeObject.tp_weaklistoffset`
      which is the offset into the instance object where weak references to the
      instance will be stored.

Support Types
`````````````

.. c:type:: destructor

   .. code-block:: c

      typedef void (*destructor)(PyObject*);

.. c:type:: getattrfunc

   .. code-block:: c

      typedef PyObject* (*getattrfunc)(PyObject* self, char* attr_name);

.. c:type:: setattrfunc

   .. code-block:: c

      typedef PyObject* (*setattrfunc)(PyObject* self, char* attr_name, PyObject* value);

.. c:type:: reprfunc::

   .. code-block:: c

      typedef PyObject* (*reprfunc)(PyObject*);

.. c:type:: inquiry

   .. code-block:: c

      typedef int (*inquiry)(PyObject*);

.. c:type:: newfunc

   .. code-block:: c

      typedef PyObject* (*newfunc)(PyObject* self, PyObject* args, PyObject* kwargs);

.. c:type:: allocfunc

   .. code-block:: c

      typedef PyObject* (*allocfunc)(PyTypeObject* cls, Py_ssize_t nitems);

.. c:type:: initproc

   .. code-block:: c

      typedef int (*initproc)(PyObject* self, PyObject* args, PyObject* kwargs);

.. c:type:: unaryfunc

   .. code-block:: c

      typedef PyObject* (*unaryfunc)(PyObject*);

.. c:type:: binaryfunc

   .. code-block:: c

      typedef PyObject* (*binaryfunc)(PyObject*, PyObject*);

.. c:type:: ternaryfunc

   .. code-block:: c

      typedef PyObject* (*ternaryfunc)(PyObject*, PyObject*, PyObject*);

.. c:type:: lenfunc

   .. code-block:: c

      typedef Py_ssize_t (*lenfunc)(PyObject*);

.. c:type:: ssizeargfunc

   .. code-block:: c

      typedef PyObject* (*ssizeargfunc)(PyObject*, Py_ssize_t);

.. c:type:: ssizeobjargproc

   .. code-block:: c

      typedef int (*ssizeobjargproc)(PyObject* Py_ssize_t, PyObject*);

.. c:type:: objobjproc

   .. code-block:: c

      typedef int (*objobjproc)(PyObject*, PyObject*);

.. c:type:: hashfunc

   .. code-block:: c

      typedef Py_hash_t (*hashfunc)(PyObject*);

.. c:type:: getattrofunc

   .. code-block:: c

      typedef PyObject* (*getattrofunc)(PyObject*, PyObject*);


.. c:type:: setattrofunc

   .. code-block:: c

      typedef PyObject* (*getattrofunc)(PyObject*, PyObject*, PyObject*);

.. c:type:: richcmpfunc

   .. code-block:: c

      typedef PyObject* (*richcmpfunc)(PyObject* lhs, PyObject* rhs, int op);


``PyLongObject``
~~~~~~~~~~~~~~~~

.. c:type:: PyLongObject

   https://docs.python.org/3/c-api/long.html

   ``PyLongObject`` is the structure which holds Python :class:`int`
   objects. This is called a ``PyLongObject`` as a hold over from when arbitrary
   width integer was called a ``long`` object in Python 2.

Casting Rules
`````````````

A ``PyLongObject*`` can safely be cast to a :c:type:`PyObject*`.

:c:type:`PyObject*`\s can be cast to ``PyLongObject*``\s only after a
:c:func:`PyLong_Check`.

API Functions
`````````````

.. c:function:: int PyLong_Check(PyObject* ob)

   Check if the instance is a :c:type:`PyLongObject`.

   :param PyObject* ob: The object to check.
   :return: True if ``ob`` is an instance of :c:type:`PyLongObject` or an
            instance of a subclass of :c:type:`PyLongObject`.

.. c:function:: unsigned long PyLong_AsUnsignedLong(PyObject* ob)

   Convert a ``PyObject*`` of type :c:type:`PyLongObject*` to an ``unsigned
   long``. If ``ob`` is not a :c:type:`long` object, an exception is raised.

   :param PyObject* ob: The object to convert.
   :return: ``ob`` as an :c:type:`unsigned long`.

.. c:function:: PyObject* PyLong_FromUnsignedLong(unsigned long l)

   Convert an :c:type:`unsigned long` into a :c:type:`PyObject*`. If the object
   cannot be allocated an exception is raised.

   :param l: The unsigned long to convert to a pyobjectptr.
   :return: A :ref:`new reference <new-reference>` to ``l`` as a Python
            object.

``PyListObject``
~~~~~~~~~~~~~~~~

.. c:type:: PyListObject

   https://docs.python.org/3/c-api/list.html

   ``PyListObject`` is the structure which holds Python :class:`list` objects.

Casting Rules
`````````````

A ``PyListObject*`` can safely be cast to a :c:type:`PyObject*`.

:c:type:`PyObject*`\s can be cast to ``PyListObject*``\s only after a
:c:func:`PyList_Check`.

API Functions
`````````````

.. c:function:: int PyList_Check(PyObject* ob)

   Check if the instance is a :c:type:`PyListObject`.

   :param PyObject* ob: The object to check.
   :return: True if ``ob`` is an instance of :c:type:`PyListObject` or an
            instance of a subclass of :c:type:`PyListObject`.

.. c:function:: PyObject* PyList_New(Py_ssize_t len)

   Create a new list object of length ``len``. The members are set to
   :c:data:`NULL` and need to be filled with :c:func:`PyList_SET_ITEM` before
   returning the list to Python.

   :param Py_ssize_t len: The length of the list to create.
   :return: A :ref:`new reference <new-reference>` to a
            :c:type:`PyListObject`. The elements must be filled in.

.. c:function:: Py_ssize_t PyList_Size(PyObject* list)

   Return the length of the list with error checking.

   :param PyObject* list: The list to get the size of.
   :return: The size of the list or -1 with an exception set if an error
            occurred. If ``list`` is not actually a :c:type:`PyListObject` an
            error will be raised.

.. c:function:: Py_ssize_t PyList_GET_SIZE(PyObject* list)

   Return the length of the list **without** error checking.

   :param PyObject* list: The list to get the size of.
   :return: The size of the list. This function **cannot** be called on objects
            that are not known to be :c:type:`PyListObject`\s.

.. c:function:: PyObject* PyList_GetItem(PyObject* list, Py_ssize_t ix)

   Lookup an item in a list with error checking.

   :param PyObject* list: The list to get an element in.
   :param Py_ssize_t ix: The index to lookup. Negative indices are not
                         supported.
   :return: A :ref:`borrowed reference <borrowed-reference>` to the element at
            index ``ix`` or :c:data:`NULL` with an :c:data:`PyExc_IndexError`
            set if ``ix`` is out of bounds.

.. c:function:: PyObject* PyList_GET_ITEM(PyObject* list, Py_ssize_t ix)

   Lookup an item in a list **without** error checking.

   :param PyObject* list: The list to get an element in.
   :param Py_ssize_t ix: The index to lookup. Negative indices are not
                         supported.
   :return: A :ref:`borrowed reference <borrowed-reference>` to the element at
            index ``ix``. It is undefined behavior if ``ix`` is out of bounds
            for ``list``.

.. c:function:: int PyList_SetItem(PyObject* list, Py_ssize_t ix, PyObject* value)

   Assign an item in a list.

   :param PyObject* list: The list to set the element in.
   :param Py_ssize_t ix: The index to assign to. Negative indices are not
                         supported.
   :param PyObject* value: The value to assign. This reference is :ref:`stolen
                           <reference-stealing>`. The old reference at ``ix``
                           will be released.
   :return: False on success or True with an exception set if an error occurred.

.. c:function:: void PyList_SET_ITEM(PyObject* list, Py_ssize_t ix)

   Lookup an item in a list **without** error checking.

   :param PyObject* list: The list to set the element in.
   :param Py_ssize_t ix: The index to assign to. Negative indices are not
                         supported. ``ix`` must be in bounds for ``list``.
   :param PyObject* value: The value to assign. This reference is :ref:`stolen
                           <reference-stealing>`. The old reference at ``ix``
                           will be **not** released and will be leaked if not
                           :c:data:`NULL`.

.. c:function:: int PyList_Insert(PyObject* list, Py_ssize_t ix, PyObject* item)

   The C API equivalent of :meth:`list.insert`. Inserts ``item`` into ``list``
   before ``ix``.

   :param PyObject* list: The list to insert into.
   :param Py_ssize_t ix: The index to insert before.
   :param PyObject* item: The item to insert.
   :return: False on success or True with an exception set if an error occurred.

.. c:function:: int PyList_Append(PyObject* list, PyObject* item)

   The C API equivalent of :meth:`list.append`. Append ``item`` to the end of
   ``list``.

   :param PyObject* list: The list to append to.
   :param PyObject* item: The item to append to ``list``.
   :return: False on success or True with an exception set if an error occurred.

.. c:function:: PyObject* PyList_GetSlice(PyObject* list, Py_ssize_t start, Py_ssize_t stop)

   Get the sub-list from ``start`` to ``stop``. This is like the Python
   expression: ``list[start:stop]``.

   :param PyObject* list: The list to slice.
   :param Py_ssize_t start: The start index of the slice. Negative indices are
                            not supported.
   :param Py_ssize_t stop: The stop index of the slice. Negative indices are not
                           supported.
   :return: A :ref:`new reference <new-reference>` to ``list[start:stop]``
            or NULL with an exception set if an error occurred.

.. c:function:: int PyList_SetSlice(PyObject* list, Py_ssize_t start, Py_ssize_t stop, PyObject* itemlist)

   Set the sub-list from ``start`` to ``stop``. This is like the Python
   statement: ``list[start:stop] = itemlist`` or ``del list[start:stop]`` if
   ``itemlist=NULL``.

   :param PyObject* list: The list to set the slice of.
   :param Py_ssize_t start: The start index of the slice. Negative indices are
                            not supported.
   :param Py_ssize_t stop: The stop index of the slice. Negative indices are not
                           supported.
   :param PyObject* itemlist: A list of items to assign to the slice. If
                              ``itemlist`` is :c:data:`NULL`, this will delete
                              the slice from the list.
   :return: False on success or True with an exception set if an error occurred.

.. c:function:: int PyList_Sort(PyObject* list)

   The C API equivalent of :meth:`list.sort`. Sorts ``list`` in place.

   :param PyObject* list: The list to sort.
   :return: False on success or True with an exception set if an error occurred.

.. c:function:: int PyList_Reverse(PyObject* list)

   The C API equivalent of :meth:`list.reverse`. Reverses a list in place.

   :param PyObject* list: The list to reverse.
   :return: False on success or True with an exception set if an error occurred.

``PyMethodDef``
~~~~~~~~~~~~~~~

.. c:type:: PyMethodDef

   https://docs.python.org/3.6/c-api/structures.html#c.PyMethodDef

   Structure used to describe a method of an extension type. This structure has
   four fields.

   .. c:member:: char* ml_name

      The name of the method as a C string.

   .. c:member:: PyCFunction ml_meth

      A pointer to the C implmenetation of the method.

   .. c:member:: int ml_flags

      Flag bits indicating how to call :c:member:`~PyMethodDef.ml_meth` should
      be called or bound to a class.

   .. c:member:: char* ml_doc

      The contents of the method's docstring as a C string. If this is
      :c:data:`NULL`, the docstring will be ``None`` in Python. This should be
      created with :c:func:`PyDoc_STRVAR`.

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
   second parameter will be :c:data:`NULL`.

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
   an instance method. The first argument will always be :c:data:`NULL`.

.. c:macro:: METH_COEXIST

   Allow a method to have the same name as an automatically generated
   wrapper. This is useful for providing a specialized version of data model
   model method like ``__contains__`` when implementing the C-API version like
   ``sq_contains``.

   .. note::

      This flag is not used often and can be ignored when writing most
      extensions.

``PyCFunction``
~~~~~~~~~~~~~~~

.. c:type:: PyCFunction

   Type of the functions used to implement most Python callables in C. Functions
   of this type take two :c:type:`PyObject*` parameters and return one such
   value. If the return value is :c:data:`NULL`, an exception shall have been
   set. If not :c:data:`NULL`, the return value is interpreted as the return
   value of the function as exposed in Python. The function must return a new
   reference.

``PyModuleDef``
~~~~~~~~~~~~~~~

.. c:type:: PyModuleDef

   A specification of a Python module object.

   .. c:member:: PyModuleDef_Base m_base

      The part of the module definition managed by CPython. Always initialize
      this with ``PyModuleDef_HEAD_INIT``.

   .. c:member:: char* m_name

      The name of the module as a C string.

   .. c:member:: char* m_doc

      The docstring of the module as a C string. If set to :c:data:`NULL` this
      will be ``None`` in Python This should be created with
      :c:func:`PyDoc_STRVAR`.

   .. c:member:: Py_ssize_t m_size

      The size of the module's global state. If set to -1 this will not allocate
      any space for global state.

   .. c:member:: PyMethodDef* m_methods

      A :c:data:`NULL` terminated array of methods to put in this module.

   .. c:member:: PyModuleDef_Slot* m_slots

      A :c:data:`NULL` terminated array of slots for using multi-phase
      initialization. This is not used in this tutorial and can be set to
      :c:data:`NULL`.

   .. c:member:: traverseproc m_traverse

      The function used for traversing the global state allocated with
      :c:member:`~PyModuleDef.m_size`. This can be :c:data:`NULL` if not needed.

   .. c:member:: inquiry m_clear

      The function used for clearing the global state allocated with
      :c:member:`~PyModuleDef.m_size`. This can be :c:data:`NULL` if not needed.

   .. c:member:: freefunc m_free

      The function used for freeing the global state allocated with
      :c:member:`~PyModuleDef.m_size`. This can be :c:data:`NULL` if not needed.

``PyMODINIT_FUNC``
~~~~~~~~~~~~~~~~~~

.. c:macro:: PyMODINIT_FUNC

   The type of the module initialization function. This function should return a
   new module object or :c:data:`NULL` if the file cannot be imported. The
   function needs to be named: ``PyInit_{name}`` where ``name`` is the name of
   the module.

Global Sentinels
----------------

.. c:var:: PyObject* Py_None

   A global reference to ``None``.

   .. c:macro:: Py_RETURN_NONE

      Shorthand for:

      .. code-block:: c

         Py_INCREF(Py_None);
         return Py_None;

      This is useful because people often forget that they need to call
      :c:func:`Py_INCREF` on :c:data:`Py_None` even though it is a global
      object.

.. c:var:: PyObject* Py_True

   A global reference to ``True``.

   .. c:macro:: Py_RETURN_TRUE

      Shorthand for:

      .. code-block:: c

         Py_INCREF(Py_True);
         return Py_True;

      This is useful because people often forget that they need to call
      :c:func:`Py_INCREF` on :c:data:`Py_True` even though it is a global
      object.

.. c:var:: PyObject* Py_False

   A global reference to ``False``.

   .. c:macro:: Py_RETURN_FALSE

      Shorthand for:

      .. code-block:: c

         Py_INCREF(Py_False);
         return Py_False;

      This is useful because people often forget that they need to call
      :c:func:`Py_INCREF` on :c:data:`Py_False` even though it is a global
      object.

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

``PyModule_Create``
~~~~~~~~~~~~~~~~~~~

.. c:function:: PyObject* PyModule_Create(PyModuleDef* def)

   Create a new Python module object from a :c:type:`PyModuleDef*`.

``PyType_Ready``

.. c:function:: int PyType_Ready(PyTypeObject* type)

   Ready a type by copying all of the slots down from the base class. This
   function should be called from the :c:macro:`PyMODINIT_FUNC`.

   :param PyTypeObject* type: The type to ready.
   :return: True with an exception set if an error occurred, otherwise False.

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
   :return: The attribute name ``attr_name`` on ``ob`` or :c:data:`NULL` with an
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
                            :c:type:`PyCFunction`. This *can* be :c:data:`NULL`.
   :param const char* format: The format string. see :ref:`format characters
                              <arg-format>` for more information.
   :param char** keywords: The names of the keyword arguments that this function
                           accepts as a :c:data:`NULL` terminated array.
   :param ...: Variadic values based on ``format``.
   :return: True with an exception set if an error occurred, otherwise False.


Example
```````

The following example defines a function called ``function_name`` which accepts
three arguments:

- ``a``: A Python ``str`` object to be converted into a :c:type:`char*`.
- ``b``: A Python :c:type:`int` object to be converted into an :c:type:`int`.
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

Accept a ``str`` argument as a :c:type:`char*`. A reference to a :c:type:`char*`
should appear in the variadic argument list at this index.

``z`` (str) [const char*]
'''''''''''''''''''''''''

Accept a ``str`` argument as a :c:type:`char*`. A reference to a :c:type:`char*`
should appear in the variadic argument list at this index. This argument can
also be ``None`` in which case the pointer will be :c:data:`NULL`.

``b`` (int) [unsigned char]
'''''''''''''''''''''''''''

Accept an :c:type:`int` argument as an :c:type:`unsigned char`. A reference to
an ``unsigned char`` should appear in the variadic argument list at this index.

``h`` (int) [short]
'''''''''''''''''''

Accept an :c:type:`int` argument as a :c:type:`short`. A reference to a
:c:type:`short` should appear in the variadic argument list at this index.

``i`` (int) [int]
'''''''''''''''''

Accept an :c:type:`int` argument as an :c:type:`int`. A reference to an
:c:type:`int` should appear in the variadic argument list at this index.

``l`` (int) [long]
''''''''''''''''''

Accept an :c:type:`int` argument as a :c:type:`long`. A reference to a
:c:type:`long` should appear in the variadic argument list at this index.

``n`` (int) [Py_ssize_t]
''''''''''''''''''''''''

Accept an :c:type:`int` argument as a :c:type:`Py_ssize_t`. A reference to a
:c:type:`Py_ssize_t` should appear in the variadic argument list at this index.

``C`` (str of length 1) [int]
'''''''''''''''''''''''''''''

Accept a ``str`` of length 1 argument as an :c:type:`int`. A reference to an
:c:type:`int` should appear in the variadic argument list at this index.

``f`` (float) [float]
'''''''''''''''''''''

Accept a ``float`` argument as a :c:type:`float`. A reference to a
:c:type:`float` should appear in the variadic argument list at this index.

``d`` (float) [double]
''''''''''''''''''''''

Accept a ``float`` argument as a :c:type:`double`. A reference to a
:c:type:`double` should appear in the variadic argument list at this index.

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
:c:type:`int` should appear in the variadic argument list at this index. This is
like accepting an object as ``O`` and then using :c:func:`PyObject_IsTrue`.

.. warning::

   The CPython docs mention converters for ``unsigned`` integral types which do
   **not** do overflow checking. These converters should not be used because
   they fail to handle negative integers. The proper way to handle these values
   is to accept them as ``O`` and use one of the ``PyLong_As*`` conversion
   functions.

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

   :return: A new reference to ``a + b`` or :c:data:`NULL` with an exception set.

.. c:function:: PyObject* PyNumber_Subtract(PyObject* a, PyObject* b)

   :return: A new reference to ``a - b`` or :c:data:`NULL` with an exception set.

.. c:function:: PyObject* PyNumber_Subtract(PyObject* a, PyObject* b)

   :return: A new reference to ``a * b`` or :c:data:`NULL` with an exception set.

.. c:function:: PyObject* PyNumber_FloorDivide(PyObject* a, PyObject* b)

   :return: A new reference to ``a // b`` or :c:data:`NULL` with an exception set.

.. c:function:: PyObject* PyNumber_TrueDivide(PyObject* a, PyObject* b)

   :return: A new reference to ``a / b`` or :c:data:`NULL` with an exception set.

.. c:function:: PyObject* PyNumber_Remainder(PyObject* a, PyObject* b)

   :return: A new reference to ``a % b`` or :c:data:`NULL` with an exception set.

.. c:function:: PyObject* PyNumber_Divmod(PyObject* a, PyObject* b)

   :return: A new reference to ``divmod(a, b)`` or :c:data:`NULL` with an
            exception set.

.. c:function:: PyObject* PyNumber_Power(PyObject* a, PyObject* b, PyObject* c)

   :param PyObject* a: The base.
   :param PyObject* b: The exponent.
   :param PyObject* c: Number to take the exponent modulo. If provided, this
                       function is like ``(a ** b) % c``. To ignore this value
                       pass :c:data:`Py_None`.
   :return: A new reference to ``pow(a, b, c)`` or :c:data:`NULL` with an
            exception set.

.. c:function:: PyObject* PyNumber_Negative(PyObject* a)

   :return: A new reference to ``-a`` or :c:data:`NULL` with an exception set.

.. c:function:: PyObject* PyNumber_Positive(PyObject* a)

   :return: A new reference to ``+a`` or :c:data:`NULL` with an exception set.

.. c:function:: PyObject* PyNumber_Absolute(PyObject* a)

   :return: A new reference to ``abs(a)`` or :c:data:`NULL` with an exception
            set.

.. c:function:: PyObject* PyNumber_Invert(PyObject* a)

   :return: A new reference to ``~a`` or :c:data:`NULL` with an exception set.

.. c:function:: PyObject* PyNumber_Lshift(PyObject* a, PyObject* b)

   :return: A new reference to ``a << b`` or :c:data:`NULL` with an exception
            set.

.. c:function:: PyObject* PyNumber_Rshift(PyObject* a, PyObject* b)

   :return: A new reference to ``a >> b`` or :c:data:`NULL` with an exception
            set.

.. c:function:: PyObject* PyNumber_And(PyObject* a, PyObject* b)

   .. note::

      This is bitwise ``and``, not boolean ``and``.

   :return: A new reference to ``a & b`` or :c:data:`NULL` with an exception
            set.

.. c:function:: PyObject* PyNumber_Xor(PyObject* a, PyObject* b)

   :return: A new reference to ``a ^ b`` or :c:data:`NULL` with an exception
            set.

.. c:function:: PyObject* PyNumber_Or(PyObject* a, PyObject* b)

   .. note::

      This is bitwise ``or``, not boolean ``or``.

   :return: A new reference to ``a | b`` or :c:data:`NULL` with an exception
            set.

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
   :return: Always :c:data:`NULL`.

.. c:function:: PyObject* PyErr_NoMemory()

   https://docs.python.org/3.6/c-api/exceptions.html#c.PyErr_NoMemory

   Raise an exception that indicates that memory could not be allocated.

   :return: Always :c:data:`NULL`.

.. c:function:: PyObject* PyErr_Occurred()

   https://docs.python.org/3.6/c-api/exceptions.html#c.PyErr_Occurred

   Return a :ref:`borrowed reference <borrowed-reference>` to the type of the
   currently raised exception. If no exception is raised, return :c:data:`NULL`.

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
