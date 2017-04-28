What is an Extension Module?
============================

A CPython extension module is a module which can be imported and used from
within Python which is written in another language. Extension modules are almost
always written in C, and sometimes in C++, because CPython provides an API for
working with Python objects targeted at C.

Why Write an Extension Module?
------------------------------

The two primary motivations for writing extension modules are:

1. Interfacing with external libraries.
2. Writing numeric or performance critical code.

Interfacing With External Libraries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Many existing libraries expose a C API. Depending on the complexity of a
library, it is often easier to write an extension module to interface with the
existing C API than to rewrite the whole package in Python.

.. warning::

   Before writing an extension module to wrap a library, check to see if the
   project already exists. Many popular libraries have already been wrapped so
   there is no reason to write a new library.

Writing Numeric or Performance Critical Code
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Another reason to write a C extension is to perform numeric calculations or
optimize a particular function. Python is fast enough for many tasks but
iterating of large arrays adds up quickly. C is well suited for large
number-crunching tasks.

One common pattern in C extensions is to provide optimized data structures and
methods written in C to be consumed by Python.

.. warning::

   Before writing an extension module to define a new numeric function or data
   structure, check to see if this already exists in numpy_, scipy_, or
   pandas_. These packages are efficient and well exercised. Most numeric code
   can be better optimized by more efficiently using numpy before writing a new
   C extension module. Other tools to consider when optimizing numeric code are
   numexpr_, or numba_. For some use cases, these tools even provide better
   performance than hand-written C or C++.

.. _numpy: http://www.numpy.org/
.. _scipy: https://www.scipy.org/
.. _pandas: http://pandas.pydata.org/
.. _numexpr: https://github.com/pydata/numexpr
.. _numba: http://numba.pydata.org/
