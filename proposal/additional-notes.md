# Structure

The talk is strucured like a workshop where I will explain a concept and then
have the students implement a concrete example. The tutorial uses two running
examples: A fibonacci function to cover the basics of implementing a function,
and a Queue class to cover the creation of types and methods. These are supposed
to be simple enough that the actual problem is not hard to solve but provide
enough work that students can focus on practicing particular skills.

Before arrival students will be expected to have downloaded some prepared
material which they will work on during the tutorial. They will also need
Python, A C compiler, and gdb. I currently plan on people installing these
packages themselves but if that slows me down during my practice runs then I
will provide some sort of prebuild environment.

# Assistants

I would love some assistants to help people during the exercise sections and
help decipher error messages for people. Assistants would probably need C
experience to help with compiler errors so if no one is available that is okay.

# No Windows

I am not covering windows because I don't have enough experience with windows
issues. I could relax this constraint if there was an assistant with experience
building extensions on windows. Even with an assistant I am worried that we will
spend too much time helping people setup their build environment on windows and
we will not get through all the content.

# Subject Experience

I am the author of many CPython extension modules of varying complexity:

- A lazy evaluation model for Python: https://github.com/llllllllll/lazy_python
- Implementation of collections.namedtuple in C:
  https://github.com/llllllllll/cnamedtuple
- Lego NXT bluetooth controller: https://github.com/llllllllll/pynxt
- Coroutine utility functions: https://github.com/llllllllll/cotoolz
- C++ wrapper for CPython API (incomplete): https://github.com/llllllllll/libpy
- Forth implementation on the CPython VM: https://github.com/llllllllll/phorth
- Embed C source code in a Python file (small extension work):
  https://github.com/llllllllll/quasiquotes

I have also contributed C extension code to some popular open source projects:

- https://github.com/Blosc/python-blosc
- https://github.com/pandas-dev/pandas

I have also contributed C code to CPython including a performance improvment to
property access.

# Past Speaking

I gave a talk at PyCon 2016 with Scott Sanderson, Playing with Python Bytecode:
https://www.youtube.com/watch?v=mxjv9KqzwjI. We also gave an extended version of
this talk at PyGotham 2016: https://www.youtube.com/watch?v=rpZz_emWIQA. I am
the PSF Certified Bytecode Expert(tm).

I have also given an introduction to classes in Python at the Boston Python User
Group in 2015: https://pixability.wistia.com/medias/kmx3ju8q94
