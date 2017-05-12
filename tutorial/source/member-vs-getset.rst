:c:type:`PyMemberDef` vs :c:type:`PyGetSetDef`
==============================================

Both :c:type:`PyMemberDef` and :c:type:`PyGetSetDef` can be used to expose
members of our C instance structure to Python, and their differences are
somewhat subtle.

:c:type:`PyMemberDef`
---------------------

:c:type:`PyMemberDef` is the simpler of the two options. A :c:type:`PyMemberDef`
defines a name for the attribute, the type as an enum value, the offset of
the member to lookup, and whether the member is read only or read-write.

:c:type:`PyMemberDef` is ideal for exposing simple attributes of an
instance. For example, imagine we have an instance structure like:

.. code-block:: c

   typedef struct {
       PyObject base;
       float float_field;
       PyObject* object_field;
   } mytype;

We may want to make these fields accessible to Python using normal attribute
access like: ``mytype.float_field`` or ``mytype.object_field``. These can be
exported with :c:type:`PyMemberDef` structures like:

.. code-block:: c

   PyMemberDef members[] = {
       {"float_field",  /* name */
        offsetof(mytype, float_field),  /* offset */
        T_FLOAT,  /* type */
        READONLY,  /* flags */
        NULL  /* docstring */},
       {"object_field",  /* name */
        offsetof(mytype, object_field),  /* offset */
        T_OBJECT_EX,  /* type */
        0,  /* flags (0 means read-write) */
        NULL  /* docstring */},
       {NULL},
   };

We would then set this array as the value of
:c:member:`PyTypeObject.tp_members` to expose these attributes to Python.

:c:type:`PyGetSetDef`
---------------------

A :c:type:`PyGetSetDef` is the more powerful option for adding attributes to a C
extension type. :c:type:`PyGetSetDef` allows us to register functions to be
called to get or set the value of an attribute. This is like :class:`property`
in Python.

:c:type:`PyGetSetDef` is useful for enforcing constraints on writes, or just
computing attributes on the fly. Given the same struct ``mytype`` from before,
we could add an attribute which could only store positive values.

.. code-block:: c

   static PyObject*
   mytype_get_float_field(mytype* self, void* closure)
   {
       return PyFloat_FromFloat(self->float_field);
   }

   static int
   mytype_set_float_field(mytype* self, PyObject* value, void* closure)
   {
       float f;

       if (!value) {
           PyErr_SetString(PyExc_AttributeError, "cannot delete float_field");
       }

       f = PyFloat_AsFloat(value);
       if (PyErr_Occurred()) {
           return -1;
       }

       if (f < 0) {
           PyErr_Format(PyExc_ValueError,
                        "float_field must be positive, got: %f",
                        f);
           return -1;
       }

       self->float_field = f;
       return 0;
   }

   PyGetSetDef getsets[] = {
       {"float_field",  /* name */
        (getter) mytype_get_float_field,
        (setter) mytype_set_float_field,
        NULL,  /* doc */
        NULL /* closure */},
       {NULL}
   };

The power of this :c:type:`PyGetSetDef` is in the setter function. This function
accepts ``self`` and the value to set as a :c:type:`PyObject*`. The function
ensures that it has received a float and that the float is positive and then
stores it on ``self``. As you can imagine, you can implement any kind of complex
logic that you would like in this function.

The ``closure`` that is passed to both the getter and setter is some arbitrary
pointer to be interpreted by your function. This is useful for writing a single
getter or setter function which can be used with some configuration. For
example, imagine ``mytype`` had three float fields that all wanted this same
validation. We could pass the offset of the particular field as the closure so
that the ``setter`` knew which field to set.

Expose ``q_maxsize`` to Python
------------------------------

As an exercise, expose the ``q_maxsize`` field of our ``Queue`` class to Python
under the name ``maxsize``.

Start by implementing this as a :c:type:`PyMemberDef`.

Once you have ``maxsize`` working as a :c:type:`PyMemberDef`, try writing it as
a :c:type:`PyGetSetDef`. Add logic in the setter to prevent the maxsize from
going below the current size.
