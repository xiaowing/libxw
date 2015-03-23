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

#define _createnode_body(type, nodetype, a) \
{ \
    LIBXW_DATANODE *headnode = NULL; \
    switch (a){ \
    case NODE_HEADNODE_STACK:   \
    case NODE_HEADNODE_QUEUE:   \
    case NODE_DATANODE_SPARE:   \
        return NULL;    \
        }   \
    headnode = get_next_available_node(GLOBAL_BLOCK_TABLE); \
    if (headnode != NULL){  \
        headnode->datatype = (NODE_HEADNODE_##nodetype | a);  \
        }   \
    return (type)headnode;  \
}

#ifdef WIN32
#define _createnode(type, name, nodetype, atype, a) \
type __cdecl name(atype a) \
_createnode_body(type, nodetype, a)
#else
#define _createnode(type, name, nodetype, atype, a) \
type  name(atype a) \
_createnode_body(type, nodetype, a)
#endif

#define _clearnodes_body(nodetype, a) \
{ \
    LIBXW_DATANODE *headnode = NULL, *cur = NULL; \
    if (a == NULL) return LIBXW_ERRNO_NULLSTACK; \
    headnode = (LIBXW_DATANODE *)a; \
    if ((headnode->datatype & NODE_HEADNODE_##nodetype) == 0) \
        return LIBXW_ERRNO_INVALID_NODETYPE; \
    while (headnode->next != NULL){ \
        cur = remove_current_node_from_list(headnode->next); \
        put_datanode_into_spare(GLOBAL_BLOCK_TABLE, cur); \
    } \
    return EXIT_SUCCESS; \
}

#ifdef WIN32
#define _clearnodes(type, name, nodetype, atype, a) \
type __cdecl name(atype a) \
_clearnodes_body(nodetype, a)
#else
#define _clearnodes(type, name, nodetype, atype, a) \
type name(atype a) \
_clearnodes_body(nodetype, a)
#endif

#define _diposehead_body(a) \
{ \
    LIBXW_DATANODE *headnode = NULL; \
    int ret = 0; \
    if ((ret = stack_clear(a)) < 0) return ret; \
    headnode = (LIBXW_DATANODE *)a; \
    put_datanode_into_spare(GLOBAL_BLOCK_TABLE, headnode); \
    return EXIT_SUCCESS; \
}

#ifdef WIN32
#define _diposehead(type, name, atype, a) \
type __cdecl name(atype a) \
_diposehead_body(a)
#else
#define _diposehead(type, name, atype, a) \
type name(atype a) \
_diposehead_body(a)
#endif

/* The declearation of the interfaces. */
int convstr(char *);
int trimstr(char *);
LIBXW_MANAGED_STACK stack_create(LIBXW_VALUE_TYPE);
int stack_dispose(LIBXW_MANAGED_STACK);
int stack_count(LIBXW_MANAGED_STACK);
int stack_peek(LIBXW_MANAGED_STACK, LIBXW_VALUE_TYPE, void *, int *);
int stack_pop(LIBXW_MANAGED_STACK, LIBXW_VALUE_TYPE, void *, int *);
int stack_push(LIBXW_MANAGED_STACK, LIBXW_VALUE_TYPE, void *, int);

#endif
