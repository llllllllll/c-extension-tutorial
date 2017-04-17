# How to Write and Debug C Extension Modules

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
