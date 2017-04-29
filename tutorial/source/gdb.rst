.. _gdb:

Debugging with GDB
==================

Debuggers are an important tool when developing software, and this is true for C
extensions as well. If anything, it is more important for a C extension than
Python code because there is no repl to prototype your code.

Debugging Enabled Python
------------------------

CPython does not come with debugging symbols or assertions by default. These
features are critical for effectively debugging CPython extensions. The
``etc/setup-env`` script builds a debug enabled version of CPython. If you are
interested in the steps check out that script.

General
-------

Debuggers let us stop execution of a function to inspect the state of the world
or poke at values to see what happens. In order to debug one of our extensions,
we need to run the entire CPython interpreter under GDB. this can done with:

.. code-block:: bash

   $ gdb python
   GNU gdb (GDB) 7.12.1
   Copyright (C) 2017 Free Software Foundation, Inc.
   License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
   This is free software: you are free to change and redistribute it.
   There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
   and "show warranty" for details.
   This GDB was configured as "x86_64-pc-linux-gnu".
   Type "show configuration" for configuration details.
   For bug reporting instructions, please see:
   <http://www.gnu.org/software/gdb/bugs/>.
   Find the GDB manual and other documentation resources online at:
   <http://www.gnu.org/software/gdb/documentation/>.
   For help, type "help".
   Type "apropos word" to search for commands related to "word"...
   Reading symbols from python...done.
   (gdb)

This drops us in a repl where we can begin to interact with the interpreter.

Setting a Breakpoint
--------------------

The most basic thing we can do when debugging a C extension is set a breakpoint
and start stepping through the function of interest. From the GDB repl, use the
``b(reak)`` command passing in the name of the file and the line number like:

.. code-block:: text

   (gdb) b file_name.c:10
   No source file named file_name.c.
   Make breakpoint pending on future shared library load? (y or [n]) y

GDB will tell us that it doesn't know about our file yet because we haven't
linked against our module yet. It is safe to just enter ``y`` to set the
breakpoint and continue.

.. note::

   You don't need to give the full path to the file, just the name is enough.

Conditional Breakpoints
~~~~~~~~~~~~~~~~~~~~~~~

Sometimes you only want to stop given some conditions. GDB supports conditional
breakpoints with the syntax:

.. code-block:: text

   (gdb) b file_name.c:10 if {expr}

where ``{expr}`` is any valid C expression. GDB will evaluate the expression
every time it hits the given line and only stop when it evaluates to True.

Running the Program
-------------------

With some breakpoints set we need to run some code. If we have a Python file we
want to run, pass the name to the ``run`` command like:

.. code-block:: text

   (gdb) run python_file.py

The arguments to ``run`` will be forwarded to the ``python`` command and GDB
will begin running the CPython interpreter which itself is running the given
file.

Another option is to enter a repl under GDB. To do this, use ``run`` with no
arguments just like you would with the ``python`` command.

Interacting with the Program
----------------------------

Once you have stopped at a breakpoint, you should enable ``tui`` mode with:

.. code-block:: text

   (gdb) tui enable

This is sort of like ``sticky`` mode if you have used ``pdb++``. ``tui`` mode
shows us the source in one window while preserving the repl below it. It is
helpful to remember where you are in the program. On the left, some lines will
be marked with ``B+`` or ``b+``; these are the lines with breakpoints set. ``B``
means unconditional breakpoint and ``b`` means conditional breakpoint. The
current line will be highlighted and have a ``>`` next to it.

Inspecting Values
~~~~~~~~~~~~~~~~~

When stopped at a breakpoint, we can evaluate C expressions with the current
variables in scope using the ``p`` command like:

.. code-block:: text

   (gdb) p 2 + 2
   $1 = 4

The output starts with ``$<n>``, which is the saved result of the expression,
followed by the value. This shows that ``2 + 2`` evaluates to ``4``, and the
result was saved in ``$1``. We can use these saved results in other expressions
like:

.. code-block:: text

   (gdb) p $1 * 2
   $2 = 8

We can use the ``whatis`` command to find out the type of an expression. For
example:

.. code-block:: text

   (gdb) whatis 2 + 2
   type = int

We can also use saved values in ``whatis`` expressions.

Navigation
~~~~~~~~~~

Just inspecting values on a single line probably won't tell us everything we
need to know. GDB allows us to move through the program in a couple of different
ways.

``s(tep)``
``````````

``s(tep)`` steps into the current expression. This means entering a function
that is being called or advancing to the next line. For example, stepping into
``f(x)`` will move us to the first line of ``f``.

``n(ext)``
``````````

``n(ext)`` runs the current line but does **not** enter functions. For example,
running next on ``f(x)`` will just move us to the next line.

``u(ntil)``
```````````

Run the program until the execution hits a line greater than the current line or
run until it hits the specified line (with the same syntax as ``break``).

This is useful for stepping out of loops.

``fin(ish)``
````````````

Run the program until it returns from the current function.

``c(ontinue)``
``````````````

Run the program until the next breakpoint is hit.

``up``
``````

Step up a stack frame into the calling function.

``down``
````````

Step down a stack frame. This is used after calling ``up``.

Setting or Removing Breakpoints
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When stopped at a breakpoint we can add new breakpoints or remove existing
breakpoints. We can remove breakpoints with:

.. code-block:: text

   (gdb) clear file_name.c:10

Interacting with CPython
------------------------

The reason we want to use GDB instead of other debuggers like lldb is that
CPython comes with an extension for GDB for interacting with
:c:type:`PyObject*`\s. The ``etc/setup-env`` script installs the GDB extension
in the created virtualenv, if you are interested in how that is done you can
check out that script.

By default, when you evaluate a :c:type:`PyObject*` in the repl you get
something like:

.. code-block:: text

   $1 = (PyObject *) 0x7ffff6f06a80

Which is not particularly useful. With just that information we would always
need to do ``ob->ob_type->tp_name`` just to know what something is. Getting the
semantic value becomes very complicated based on the type of the object.

The CPython GDB extension includes pretty printers for objects. The pretty
printers generate output like:

.. code-block:: text

   (gdb) p (PyObject*) self
   $8 = <queue.Queue at remote 0x7ffff6f06a80>
   (gdb) p self->q_elements
   $9 = []
   (gdb) whatis self->q_elements
   type = PyObject *

For many builtin types the repr is used, for custom types the type name is
printed with the address. This makes it much easier to quickly understand the
state of your program.

Python Specific Commands
~~~~~~~~~~~~~~~~~~~~~~~~

The CPython GDB extension also includes commands for interacting with the
Python interpreter.

``py-bt``
`````````

Print the Python traceback up to the current C function.

.. note::

   The order of frames is inverted from a normal Python traceback. The most
   recent call is **first**. This is to match the traceback order used by GDB.

``py-list``
```````````

Print the Python source code of the currently selected Python frame.

``py-locals``
`````````````

Print the local variables of the currently selected Python frame.

``py-up``
`````````

Select the Python stackframe that called the currently selected frame.

``py-down``
```````````

Select the Python stackframe below the currently selected frame.

``py-print``
````````````

Print a variable in the currently selected python frame.

Fix ``Queue.rotate``
--------------------

``Queue.rotate`` currently has a reference deficit. Use GDB with the CPython
extension to step through the function and find the bug. Once you have
identified and corrected the bug, step through the function to verify your fix
works.
