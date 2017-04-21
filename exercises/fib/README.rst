Function Exercises
==================

Using the Abstract Number API
-----------------------------

Rewrite ``fib`` to use the ``PyNumber_*`` API and ``PyObject*``\s instead of
``unsigned long`` objects.

Use ``PyArg_ParseTupleAndKeywords`` to Accept a new ``a`` and ``b``.
--------------------------------------------------------------------

Rewrite ``fib`` to use ``PyArg_ParseTupleAndKeywords`` to do type
adaptation. After that, accept arguments for ``a`` and ``b`` which both default
to ``1``.
