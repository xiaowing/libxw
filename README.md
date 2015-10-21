libxw
=============
[![Build Status](https://travis-ci.org/xiaowing/libxw.svg?branch=master)](https://travis-ci.org/xiaowing/libxw)

###Summary 
A C-language library contains several utility functions and an implementation of Stack, Queue and Matrix based on a lightweight memory management.

Currently this library contains the following content.

1. Data structure of an implementation of stack and a set of functions to use the stack.
2. Data structure of an implementation of queue and a set of functions to use the queue.
3. Data structure of an implementation of matrix (based on the implementation of cross linked list) and a set of functions to use the matrix.
4. A set of utilities to deal with the C-style string(which ends with '\0'). Such as trim and reverse.

###API references
~(under construction)~

###How to build the source code?

####Linux
Build environment:
* gcc
* python(2.5+)

Build steps:

1. Compile the source.
2. Install the binary.

```
    $make
    $make install
```

The binary will be copied to /usr/lib(/usr/lib/libxw.so) and the header will be copied to /usr/include(/usr/include/libxw.h /usr/include/libxwdef.h).

####Windows
Build environment:
* Visual Studio 2013 or above

Build steps:

1. Open the libxw.sln with Visual Studio.
2. Then, you know what to do:)

###How to run the test code ?
The test code of libxw was written under the assumption that the test code would be run with CUnit framework.
However, no distribution of cunit will be delivered with the source code due to the issue of license.
If you want to run the test code, the following steps will be needed.

####Linux
1. Install CUnit. Please refer to [CUnit Home](http://cunit.sourceforge.net/).
2. Modify the Makefile to change the INCLDIR value according to the result of installation of CUnit

    for example: INCLDIR = /usr/include   # change it to the path of your Cunit headers({INCLDIR}/CUnit).

3. Compile the test program like this.
```
    $make test
```

__Note:__ 
Due to the interface change of CUnit-2.1.3, you should add a pre-definition of the macro "CUNIT_213" before compiling.
Edit Makefile with any editor you have, and modify it as follows:
```
    TESTOPT = -Wall -O2 $(TESTINCL) $(TESTLNK) -D CUNIT_213
```

####Windows
1. Build the CUnit source code in Windows.
2. Move all the headers of CUnit to the directory __test\include__.
3. Move the generated __.lib__ file(libcunit_dll.lib) to the directory __test\lib__ or __test\lib\x64__(if builded as x64 binary).
4. Move the generated __libcunit.dll__ file to the output directory such as __bin\Debug__ or __bin\Release\x64__.
5. Compile the whole solution with Visual Studio.

__Note:__ 
Due to the interface change of CUnit-2.1.3, you should add a pre-definition of the macro "CUNIT_213" before compiling.
Edit test_libxw.vcproj with any editor you have, and modify all the <PreprocessorDefinitions> sections as follows:
```
    <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;_LIB;CUNIT_213;%(PreprocessorDefinitions)</PreprocessorDefinitions>
```
