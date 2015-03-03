libxw
=============
###Summary 
A C-language library contains several utility functions and an implementation of Stack and Queue based a lightweight memory management.

Currently this library contains the following content.

1. Data structure of an implementation of stack and a set of functions to use the stack.

2. A set of utilities to deal with the C-style string(which ends with '\0'). Such as trim and reverse.

###Directory structure of the source code.
libxw

  ├─include/						headers of libxw
  
  ├─src/							source files of libxw 
  
  ├─test/ 							test code of libxw
  
  ├─libxw.def						the symbol definitions on windows
  
  ├─libxw.sln						the solution settings file for Visual Studio 2013 community
  
  ├─libxw.vcxproj					the libxw project settings file for Visual Studio 2013 community
  
  ├─libxw.vcxproj.filters
  
  ├─test_libxw.vcxproj				the test program project settings file for Visual Studio 2013 community
  
  ├─test_libxw.vcxproj.filters
  
  ├─Makefile						Makfile for compiling libxw on Linux with gcc
  
  └─LICENSE

###API references
~(under construction)~

###How to run the test code ?
The test code of libxw was written under the assumption that the test code would be run with CUnit framework.
However, no distribution of cunit will be delivered with the source code due to the issue of license.
If you want to run the test code, the following steps will be needed.

~(under construction)~


  