Common Issues and Bugs
======================

C is much less friendly than Python. Programmer errors often lead to undefined
behavior and crashes. When writing C extensions it is important to look out for
common issues that can cause hard to debug failures later.

Reference Counting Issues
-------------------------

The most common and annoying issues in C extension modules are around
:ref:`reference counting <ref-count>`. CPython uses the :ref:`reference count
<ref-count>` of an object to know when to deallocate the object, when this gets
out of sync we can have one of two kinds of problems: a reference surplus or a
reference deficit.

Reference Surplus (Leak)
~~~~~~~~~~~~~~~~~~~~~~~~

A reference surplus is when an object believes it has more references than
actually exist. This means that :c:func:`Py_DECREF` will not be called enough
times to bring the reference :ref:`reference count <ref-count>` back to zero and
the object will never be deallocated. This is commonly referred to as a memory
leak, reference leak, or leaking an object.

An easy way for a leak to occur is when calling a function which returns a
:ref:`new reference <new-reference>`. The caller of the function is responsible
for the newly returned reference, if they don't explicitly give this reference
away then the **must** call :c:func:`Py_DECREF` (or another reference
decrementing function) on the object.

Reference leaks are hard to detect in testing because, for most cases, the code
appears to work as intended. These issues only become noticeable when the memory
usage of your program grows unboundedly. A quick check you can do in a repl is
call your function in an infinite loop. This should not cause the memory usage
to grow after the interpreter has warmed up. Let the loop run for around a
minute and if the memory usage grows with time you know the function must be
leaking.

.. warning::

   The inverse is not always true: if the memory usage doesn't grow it might
   still be leaking references to shared objects.

Some objects in the interpreter are memoized but should still have an accurate
reference count. For example: :c:type:`PyLongObject`\s in the range [-5, 256]
are memoized such that all instances of ``0`` are the same object in
memory. This is also true for global sentinels like :c:data:`Py_None`,
:c:data:`Py_True`, or :c:data:`Py_False`. If you leak a reference to one of
these objects it is much harder to notice.

Reference Deficit
~~~~~~~~~~~~~~~~~

A reference deficit is when an object believes it has less references than
actually exist. This means that :c:func:`Py_DECREF` will be called too many
times and there may be a point where objects refer to a deallocated object which
they believe they are keeping alive.

An easy way for a reference deficit to occur is when calling a function that
returns a :ref:`borrowed reference <borrowed-reference>`. The caller of the
function is not responsible for the reference, so if they call
:c:func:`Py_DECREF` on it or give the reference away there will be one less
reference than expected. Reference deficits often create "use after free" bugs
where code tries to use the data stored in a :c:type:`PyObject*` which has
already been deallocated and now contains undefined data .

Reference deficits have more "explosive" failure modes than reference leaks so
it is harder to miss them. Often the failure mode of a reference deficit is that
you will get a segmentation fault at interpreter exit or in the middle of a
function. One bad thing about reference deficits is that the crash is often not
in a related location to the actual problem because it occurs when someone uses
the object after it has been deallocated.

Unlike reference leaks there isn't a quick repl test that I know of to check for
these, the best bets is to walk through your functions in a :ref:`debugger
<gdb>` and verify that the reference counts of your objects are what you
expect. It doesn't hurt to add some assertions to codify your expectations for
development builds.

A special kind of reference deficit that is easy to miss is a deficit of a
global sentinel like :c:data:`Py_None`, :c:data:`Py_True`, or
:c:data:`Py_False`. These objects need to have proper reference counts just like
any other, the only difference is that they should never get deallocated because
their :ref:`reference count <ref-count>` starts at 1. These global
sentinel values have helper macros like: :c:macro:`Py_RETURN_NONE` which
increment the reference count and return the object. These macros aren't
required but are a nice little convenience.
