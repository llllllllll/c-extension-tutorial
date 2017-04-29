Class Exercises
===============

Implement ``push`` and ``pop``
------------------------------

Fill in the implementations for ``Queue.push`` and ``Queue.pop``.

Try to implement these using the List API functions.

Fix the Bug in ``rotate``
-------------------------

``Queue.rotate`` has a reference counting bug. Use GDB with the CPython GDB
extension to find and correct the issue. Use GDB to verify your fix.

Expose ``maxsize`` to Python
----------------------------

Expose ``Queue.maxsize`` to Python first as a ``PyMemberDef`` and then as a
``PyGetSetDef``.

Answers
-------

You can compare your results to ``queue-complete.c``. There are many correct
implementations so it might not match exactly.

``queue-complete.c`` also shows how you could implement the Sequence Protocol
for ``Queue``.
