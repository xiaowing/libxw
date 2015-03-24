# makefile name : Makefile
# Author      	: Yi Yi
# Copyright		: Apache License 2.0
# Summary		: The makefile for compiling libxw on Linux with gcc 

CC = gcc
BASE = .
OUTDIR = $(BASE)/bin
OBJDIR = $(BASE)/obj
SRCDIR = $(BASE)/src
CFLAGSG = $(INCLUDE) -fPIC
INCLUDE = -I$(INC)
INC = $(BASE)/include
SOOPT = -fPIC -shared -lpthread
INSDIR = /usr/lib
LIBNAME = libxw.so
VPATH = $(SRCDIR):$(INC):$(OBJDIR):$(OUTDIR)

OBJ = convstr.o trimstr.o minivm.o stqu.o
TARGET = $(OBJ)

objects: $(TARGET)
$(TARGET): %.o: %.c
	-mkdir -p $(OBJDIR)
	$(CC) -c $(CFLAGSG) $< -o $(OBJDIR)/$@

so: $(LIBNAME)
$(LIBNAME): $(TARGET)
	-mkdir -p $(OUTDIR)
	$(CC) ${SOOPT} $(OBJDIR)/*.o -o $(OUTDIR)/$(LIBNAME)

all: objects so

install:
	cp $(OUTDIR)/$(LIBNAME) $(INSDIR)

stqu.o: minivm.o $(INC)/libxw.h $(INC)/libxwdef.h

.PHONY: clean
clean:
	-rm -rf $(OBJDIR)/*.o $(OUTDIR)/$(LIBNAME)
	-rmdir $(OUTDIR) $(OBJDIR)

