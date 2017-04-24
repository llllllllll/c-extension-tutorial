How to Write and Debug C Extension Modules
==========================================

The CPython interpreter allows us implement modules in C for performance
critical code or to interface with external libraries while presenting users
with a high level Python API. This tutorial will teach you how to leverage to
power of C in your Python projects.

We will start by explaining the C representation of Python objects and how to
manipulate them from within C. We will then move on to implementing functions in
C for use in Python. We will discuss reference counting and correct exception
handling. We will also talk about how to package and build your new extension
module so that it may be shared on PyPI. (We will only be covering building
extension modules on GNU/Linux and OSX, not Windows).

After the break, we will show how to implement a new type in C. This will cover
how to hook into various protocols and properly support cyclic garbage
collection. We will also discuss techniques for debugging C extension modules
with gdb using the CPython gdb extension.

Install Steps
-------------

Prior to the tutorial, attendees need to install a git, a C89 compatible C
compiler and gdb.

For the C compiler: I recommend gcc, but clang works as well.
The tutorial will be using gdb specific features so gdb is reqiured. Other C
debuggers like lldb will not work.

Once all of the system packages have been installed, run the following commands
in a terminal:

.. code-block:: bash

   $ git clone --recursive git@github.com:llllllllll/c-extension-tutorial
   $ cd c-extension-tutorial
   $ source etc/setup-env

The ``setup-env`` script will compile a debug version of CPython 3.6 and create
a local virtual env with this new interpreter. This ensures that everyone is
using the same version of CPython with the same compile time flags.

Viewing the Tutorial
--------------------

.. warning::

   The tutorial content is not complete! The install instructions above are
   valid but the content is not complete yet so don't read ahead!

The tutorial is structured as a sphinx project. This allows the tutorial to be
viewed from a standard browser or hosted online.

The material can be viewed in a browser by opening
``tutorial/build/html/index.html``, for example:

.. code-block:: bash

   $ ${BROWSER} tutorial/build/html/index.html
