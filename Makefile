# makefile name : Makefile
# Author        : Yi Yi
# Copyright     : Apache License 2.0
# Summary       : The makefile for compiling libxw on Linux with gcc

CC = gcc
BASE = .
OUTDIR = $(BASE)/bin
OBJDIR = $(BASE)/obj
SRCDIR = $(BASE)/src
CFLAGSG = $(INCLUDE) -fPIC
INCLUDE = -I$(INC)
INC = $(BASE)/include
SYMBOL_DEF = libxw.def
STSYMBOL_DEF = st.sym
DYSYMBOL_DEF = dy.map
LIN_DEF = $(STSYMBOL_DEF) $(DYSYMBOL_DEF)
SOOPT = -shared -lpthread -Wl,--retain-symbols-file $(STSYMBOL_DEF) -Wl,--version-script $(DYSYMBOL_DEF)
INSDIR = /usr/lib
INCLDIR = /usr/include
LIBNAME = libxw.so

TESTAPL = testlibxw
TESTINCL = $(INCLUDE) -I$(INCLDIR)/CUnit
TESTLNK = -lpthread -lxw -lcunit
TESTOPT = -Wall -O2 $(TESTINCL) $(TESTLNK)
TESTDIR = $(BASE)/test

VPATH = $(SRCDIR):$(INC):$(OBJDIR):$(OUTDIR):$(TESTDIR)

so: $(LIBNAME)
OBJ = convstr.o trimstr.o sort.o minivm.o stqu.o crosslnk.o
TARGET = $(OBJ)

objects: $(TARGET)
$(TARGET): %.o: %.c
	-mkdir -p $(OBJDIR)
	$(CC) -c $(CFLAGSG) $< -o $(OBJDIR)/$@

def: $(LIN_DEF)
$(LIN_DEF): $(SYMBOL_DEF)
	-python gen.py $(SYMBOL_DEF)

$(LIBNAME): $(TARGET) $(LIN_DEF)
	-mkdir -p $(OUTDIR)
	$(CC) $(OBJDIR)/*.o -o $(OUTDIR)/$(LIBNAME) ${SOOPT}

all: objects so

stqu.o: minivm.o $(INC)/libxw.h $(INC)/libxwdef.h

crosslnk.o: minivm.o $(INC)/libxw.h $(INC)/libxwdef.h

install:
	cp $(OUTDIR)/$(LIBNAME) $(INSDIR)
	cp $(INC)/*.h $(INCLDIR)

test: $(TESTAPL)
TESTOBJ = runtest.c teststack.c testqueue.c testmatrix.c testutil.c
$(TESTAPL): $(TESTOBJ)
	$(CC) $(TESTOPT) $(TESTDIR)/*.c -o $(BASE)/$(TESTAPL)
	
.PHONY: clean
clean:
	-rm -rf $(OBJDIR)/*.o $(OUTDIR)/$(LIBNAME) $(INSDIR)/$(LIBNAME)
	-rmdir $(OUTDIR) $(OBJDIR)
	-rm -f $(INCLDIR)/libxw.h
	-rm -f $(INCLDIR)/libxwdef.h
	-rm -f $(BASE)/$(TESTAPL)