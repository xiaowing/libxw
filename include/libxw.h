/*
============================================================================
Name        : libxw.h
Author      : Yi Yi
Version     :
Copyright   : Under Apache License 2.0
Description : The header of libxw. 
============================================================================
*/
#ifndef LIBXW_HERDER_
#define LIBXW_HERDER_

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif
#include "libxwdef.h"

typedef void * LIBXW_MANAGED_STACK;
typedef void * LIBXW_MANAGED_QUEUE;
// typedef int LIBXW_VALUE_TYPE;

typedef enum  node_value_type{
    NODE_VALUE_CHAR = 0x1,
    NODE_VALUE_INTEGER = 0x4,
    NODE_VALUE_SMALLINT = 0x5,
    NODE_VALUE_LONG = 0x6,
    NODE_VALUE_FLOAT = 0x8,
    NODE_VALUE_DOUBLE = 0x9,
    NODE_VALUE_CSTRING = 0x10,
    NODE_VALUE_CUSTOMIZED = 0x20,

    NODE_HEADNODE_STACK = 0x100,
    NODE_HEADNODE_QUEUE = 0x200,
    NODE_DATANODE_SPARE = 0x1000,
}LIBXW_VALUE_TYPE;

/*#ifdef WIN32
extern int __cdecl convstr(char *);
extern int __cdecl trimstr(char *);
LIBXW_MANAGED_STACK* __cdecl stack_create(LIBXW_VALUE_TYPE);
int __cdecl stack_dispose(LIBXW_MANAGED_STACK *);
int __cdecl stack_items_counter(LIBXW_MANAGED_STACK *);
int __cdecl stack_peek(LIBXW_MANAGED_STACK *, LIBXW_VALUE_TYPE, void *, int *);
int __cdecl stack_pop(LIBXW_MANAGED_STACK *, LIBXW_VALUE_TYPE, void *, int *);
int __cdecl stack_push(LIBXW_MANAGED_STACK *, LIBXW_VALUE_TYPE, void *, int);
#else*/
int convstr(char *);
int trimstr(char *);
LIBXW_MANAGED_STACK stack_create(LIBXW_VALUE_TYPE);
int stack_dispose(LIBXW_MANAGED_STACK);
int stack_items_counter(LIBXW_MANAGED_STACK);
int stack_peek(LIBXW_MANAGED_STACK, LIBXW_VALUE_TYPE, void *, int *);
int stack_pop(LIBXW_MANAGED_STACK, LIBXW_VALUE_TYPE, void *, int *);
int stack_push(LIBXW_MANAGED_STACK, LIBXW_VALUE_TYPE, void *, int);
/*#endif*/

#endif
