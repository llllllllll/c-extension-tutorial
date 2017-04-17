Writing a New Function in C
===========================

Let's begin with a simple C function to compute Fibonacci numbers:

.. code-block:: c

   static unsigned long
   cfib(unsigned long n)
   {
       unsigned long a = 1;
       unsigned long b = 1;
       unsigned long c;

       if (n <= 1) {
           return 1;
       }

       while (--n > 1) {
           c = a + b;
           a = b;
           b = c;
       }

       return b;
   }

Now that we have a a C function we need to provide a way to convert our
:ref:`pyobjectptr`\s to and from C ``unsigned long`` objects.

Adapting Long Objects
---------------------

For many primitive C types, CPython provides functions to convert to and from
Python objects. For ``unsigned long``, we can use
:c:func:`PyLong_FromUnsignedLong` and :c:func:`PyLong_AsUnsignedLong` to convert
to and from C unsigned long objects.
