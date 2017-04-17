How to Write and Debug C Extension Modules
==========================================

Material for my PyCon tutorial.

The material is in the ``tutorial`` directory.

What to Install Before Attending
--------------------------------

Prior to the tutorial, attendees need to install a C89 compatible C compiler and
gdb.

For the C compiler: I recommend gcc, but clang works as well.
The tutorial will be using gdb specific features so gdb is reqiured. Other C
debuggers like lldb will not work.

Building the Tutorial
---------------------

The tutorial is structured as a sphinx project. This allows the tutorial to be
viewed from a standard browser or hosted online.

To build the sphinx project, setup the environment with ``$
./etc/setup-env``. This script builds the version of CPython needed and installs
all of the needed packages.

After setting up the environment, ``cd`` into the ``tutorial`` directory and run
``$ make html``.

The material can now be viewed in a browser by opening
``tutorial/build/html/index.html``
