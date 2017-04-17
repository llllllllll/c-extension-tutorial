I will ask students to have Python 3.5, a C compiler, and gdb (not lldb)
available before the tutorial begins. Students may want to have the CPython repo
cloned for when we talk about debugging but that is not required. I will also
provide a repo for them to follow along with code examples and open ended
exercises which should be downloaded before the tutorial.

The types of sections I have are:

* Lecture: Me speaking to the group. This type of section has no active
  participation from the student.
* Follow Along: Me speaking to the group while students repeat the actions on
  their own machine.
* Exercise: I will explain a task and then let students work through a
  problem. Assistants and I would go around and help people one on one.

# Introduction (Lecture)

(3 minutes; running total: 3 minutes)

I will start by introducing myself and guaging the C experience of the
group. Based on the C experience of the group I may fall back on using more C
standard library concepts and functions like `errno` or `memcpy`.

I will then intoduce the general idea of an extension module.  I will cover some
common reasons for writing an extension module, most importantly:

1. Performance demands and hot spot optimizations
2. Wrapping libraries written in other languages

# Basics of the C Level Representations (Lecture)

(8 minutes; running total: 11 minutes)

Before we begin working on an extension module, we need to talk about how python
objects are represented to us in C. All objects are shown in C as a
`PyObject*`. The `PyObject` struct exposes two fields:

* `Py_ssize_t ob_refcnt`
* `PyTypeObject *ob_type`

Here I will go over the reference counting system. I will mention the cyclical
garbage collector but only to foreshadow work done in the classes section of the
tutorial. Here I will show `Py_INCREF`, `Py_DECREF`, `Py_XDECREF`, and
`sys.getrefcount`. At this point I will introduce the vocabulary "owned
reference" or "new reference" and "borrowed reference" which will be reinforced
as we go through the rest of the tutorial.

I will use `PyTypeObject` as an example of how the C API specializes objects
when the type is known and then explain how `ob_type` works in objects. Here I
will introduce the `Py_TYPE` macro.

# Writing a C Function (Lecture)

(2 minutes; running total: 13 minutes)

(This code will be in a prewritten C file in the prepared materials)

Let's start by writing a basic C function to compute the nth fibonacci number.

    static unsigned long
    cfib(unsigned long n)
    {
        unsigned long a = 1;
        unsigned long b = 1;
        unsigned long c;

        if (n <= 1) {
            return 1;
        }

        n -= 1;
        while (--n) {
            c = a + b;
            a = b;
            b = c;
        }

        return b;
    }

So far so good; however, how do we feed this function `unsigned long` values if
everything is a `PyObject`? Also, how can we consume the output of the function
if it is a `unsigned long` and not a `PyObject*`. We need to wrap this function
in a CPython aware function.

# Boxing and Unboxing `PyObject`s (Lecture)

(2 minutes; running total: 15 minutes)

(This code will be in a prewritten C file in the prepared materials)

Introduce `PyLongObject` (`int`) as a new specialization of `PyObject`.


    #include <Python.h>

    static PyObject *
    pyfib(PyObject *self, PyObject *n)
    {
        unsigned long asul = PyLong_AsUnsignedLong(n);
        PyObject *result = PyLong_FromUnsignedLong(cfib(asul));
        return result;
    }

# Creating Python Callables (Lecture)

(5 minutes; running total: 20 minutes)

Now that we have a function that acts on `PyObject`s we need to turn this into a
python callable object. To define the spec for this, we can use a `PyMethodDef`
struct.

We will define a struct like so:

    PyDoc_STRVAR(fib_doc, "Calculates the nth fibonacci number.");

    PyMethodDef method = {
        "fib", (PyCFunction) pyfib, METH_O, fib_doc,
    };

I will mention why `PyDoc_STRVAR` exists instead of just using a normal string.
We will then go over the fields of the `PyMethodDef` structure and briefly
discuss the options for the `ml_flags` slot.

I will explain how a method def turns into a callable through a module, type,
or `PyCFunction_NewEx` but move on quickly to show through the example of a
module.

# Creating Modules (Lecture)

(5 minutes; running total: 25 minutes)

(This code will be in a prewritten C file in the prepared materials)

Now that we have our spec, we can pack this up into a python module object. This
can be done in a similar way by defining a `PyModuleDef` struct:

    PyMethodDef methods[] = {
        {"fib", (PyCFunction) pyfib, METH_O, fib_doc},
        {NULL},
    };

    PyDoc_STRVAR(fib_doc, "Provides an nth fibonacci number function.");

    PyModuleDef fib_module = {
        PyModuleDef_HEAD_INIT,
        "fib",
        fib_doc,
        -1,
        methods,
        NULL,
        NULL,
        NULL,
        NULL
    };

Here we use the array for the method instead because the module needs an array
of methods.

I will go over the `-1` and `NULL` slots and explain how they are used with
module state.

We now need to tell python how to create our module object. The way to do this
is to define a function called `PyInit_<modname>` of type `PyMODINIT_FUNC` which
takes no arguments and returns a `PyObject*` pointing to a
`PyModuleObject`. This can be as simple as:

    PyMODINIT_FUNC
    PyInit_fib(void)
    {
        return PyModule_Create(&_fib_module);
    }

# Building (Follow Along)

(5 minutes; running total: 30)

(This code will be in a prewritten Python file in the prepared materials. I will
use this section to ensure people have their build environments working.)

To buiild our extension, the easiest thing to do is use a standard
`setup.py`. Every python project you distrubute should already have one and it
is the canonical way to install python packages. We can write a simple
`setup.py` like so:

    from setuptools import (
        Extension,
        setup,
    )

    setup(
        name='fib',
        author='PyCon Tutorial Attendee',
        url='.',
        packages=['fib'],
        ext_modules=[
            Extension(
                'fib',
                ['fib.c'],
            ),
        ],
    )

The only special thing about this `setup.py` is that we have a list of
`Extension` objects which represent the C modules we want to include. This has
the name of the module, the path to the C files needed to build it, and
optionally some extra compile args. We can then build our new module with
`$ python setup.py build_ext --inplace`. Now that this is built, we can test it
out in the repl:


    In [1]: from fib import fib

    In [2]: fib(50)
    Out[2]: 12586269025

    In [3]: %timeit fib(50)
    10000000 loops, best of 3: 78 ns per loop

Looks pretty good!

# What We Did Wrong (Follow Along)

(3 minutes; running total: 33 minutes)

In our function here we assumed the input was an int and that we could always
allocate memory. Here I will ask students to see what happens when you pass
something that is not an int like a string or dict.

This will cause their terminal to hang showing students the dangers of ignoring
error handling and input validation.

# Exceptions and Error Handling (Lecture)

(10 minutes; running total: 43 minutes)

I will start by diving into why the code hangs when passed something that is not
an int. I will explain how the exception system works and show some examples of
`PyErr_Occurred`, `PyErr_SetString` and `PyErr_Format`. Again, I will refer
students to the docs for format because there are a lot of options. Based on the
C experience of the group I will mention that this pattern is a lot like `errno`
error handling. After that I will discuss how functions return `NULL` to signal
errors except in special cases like `PyLong_AsUnsignedLong` where they cannot.

I will go over how you need to aggresivly guard against any possible failure
like `MemoryError`s which many people forget about.

# Add Error Handling to Fib Example (Exercise)

(7 minutes; running total: 50 minutes)

Ask students to add error handling to our fib example. Students should add the
code and then test it in the Python repl to make sure the correct exception is
raised.

# Abstract APIs (Lecture)

(7 minutes; running total: 57 minutes)

When testing your code before some of you may have noticed that for `n > 93`
the result overflows and you get the wrong answer. While `PyLongObject`s can
be arbitrarily large, `unsigned long` cannot. Here I will explain the idea of
the abstract object APIs like `PyObject_Repr` and `PyNumber_Add`. I will refer
to the docs for the C object protocol functions and give an overview of a
couple. These should be quick for people because most have a 1:1 relation to the
builtin python functions. I will draw special attention to
`PyObject_RichCompare` because it is not quite how comparison works in Python.

I will also show the abstract number like `PyNumber_Add` and
`PyNumber_Subtract`.

# Fix the Overflow in Fib with the Number Protocol (Exercise)

(15 minutes; running total: 1 hour, 12 minutes)

Have the students rewrite the `cfib` function in terms of the abstract number
API. Remind them to think about reference counting and error handling. Students
who need more to do can work on optimizing the function by dispatching to the C
loop or the CPython API loop based on the input.

# Fancier Argument Parsing (Lecture)

(8 minutes; running total: 1 hour, 20 minutes)

Using `METH_O` is great for quick functions or small helpers; however, for a
nicer user API we often want to support things like keyword arguments or default
arguments. Introduce `METH_VARARGS`, `METH_KEYWORDS` and
`PyArg_ParseTupleAndKeywords`. Again, I will show the docs for the format string
options. I will go over the `O` , `O!`, and `k` format codes in detail.

# Add Optional Starting Values and Keyword Arguments to Fib (Exercise)

(10 minutes (up to break); running total: 1 hour, 30 minutes)

Ask students to modify the fib function to accept new starting values (`a` and
`b`, and to accept the arguments positionally or by keyword.

# Break

(20 minutes; running total: 1 hour, 30 minutes)

Snack time.

# Classes (Lecture)

(25 minutes; running total: 1 hour, 55 minutes)

Now that we know the basics of writing a free function, we can discuss writing a
new Python class in C. Here I will cover the `PyTypeObject` structure and many
of the slots. I will refer back to the abstract object protocol functions to
show how the slots corrospond to many of those calls. I will also show how
something like `repr(x)` turns into `Py_TYPE(x)->tp_repr(x)` so people see how
the desugaring works.

Here we will cover object allocation and deallocation as well. We will also
discuss `PyType_Ready` and why that is needed.

# Add Push and Pop to a Queue Class (Exercise)

(15 minutes; running total: 2 hours, 10 minutes)

Given the beginings of a Queue class implementation, add a `push` and `pop`
method. These should properly handle a full or empty queue.

This exercise will let people work with object state and learn the
`PyListObject` API.

# Common Errors and How to Avoid Them (Lecture)

(7 minutes; running total: 2 hours 17 minutes)

Explain some common problems when working with the CPython API. I will spend a
lot of time around reference counting problems. I will show off macros like
`Py_RETURN_NONE` which help avoid reference issues at interpreter shutdown. I
will talk about debug builds of the interpreter which can help catch reference
bugs early.

# Python gdb Extension (Lecture)

(13 minutes; running total: 2 hours: 30 minutes)

I will show how to use the python-gdb.py file to assist in debugging extensions
in gdb. This will include setting up your gdbinit to load the file, installing
the extension, and an overview of the commands and usage of the python specific
gdb features.

# Fix Error in Rotate Method (Exercise)

(7 minutes; running total: 2 hours, 37 minutes)

I will have provided an implementation of `Queue.rotate` which has a reference
count issue in it. Students can spend the time using gdb to try to decipher
where the bug is. We will also help students get the extension properly
installed if needed.

# Supporting the Cyclical Garbage Collector (Lecture)

(8 minutes; running total: 2 hours, 45 minutes)

Here I will explain how reference cycles are deleted. I will go over
`tp_traverse` and `tp_clear`. I will explain when it is safe to avoid a
`tp_clear`. I will also go over the new allocation/deallocation functions that
must be used along with the new tp_flag that must be set.

# Add Cyclical GC Support to Queue (Exercise)

(5 minutes; running total: 2 hours, 50 minutes)

Have students implement a traverse and clear for the queue class.

# PyMemberDef vs PyGetSet (Lecture)

(10 minutes; running total: 3 hours)

Explain what members and getsets are. Show how to implement them and when to
select one versus the other. I will walk through implementing `maxsize` as a
member and a getset and show that as a member it must be read-only to maintain
the state of the queue; but, as a getset we can raise an error if you make the
maxsize less than the current size.
