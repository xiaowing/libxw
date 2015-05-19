libxw
=============
###Summary 
A C-language library contains several utility functions and an implementation of Stack and Queue based a lightweight memory management.

Currently this library contains the following content.

1. Data structure of an implementation of stack and a set of functions to use the stack.
2. A set of utilities to deal with the C-style string(which ends with '\0'). Such as trim and reverse.

###API references
~(under construction)~

###How to build the source code?

####Linux
Build environment:
* gcc
* python(2.5+)

Build steps:

1. Compile the source.

    $make
	
2. Install the binary.

    $make install

The binary will be copied to /usr/lib(/usr/lib/libxw.so) and the header will be copied to /usr/include(/usr/include/libxw.h /usr/include/libxwdef.h).

####Windows
Build environment:
* Visual Studio 2013 or above

Build steps:

Open the libxw.sln with Visual Studio and you know what to do.

###How to run the test code ?
The test code of libxw was written under the assumption that the test code would be run with CUnit framework.
However, no distribution of cunit will be delivered with the source code due to the issue of license.
If you want to run the test code, the following steps will be needed.

1. Install CUnit. Please refer to [CUnit Home](http://cunit.sourceforge.net/).
2. Compile the test program.

    $make test


  