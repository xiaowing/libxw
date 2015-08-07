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
typedef void * LIBXW_MANAGED_MATRIX;
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
    NODE_HEADNODE_CLINK_COLHEAD = 0x1000,
    NODE_HEADNODE_CLINK_ROWHEAD = 0x2000,
    NODE_HEADNODE_CLINK = 0x4000,
    NODE_DATANODE_SPARE = 0x8000,
}LIBXW_VALUE_TYPE;

typedef enum enum_bool{
    BOOLEAN_FALSE = 0x00,
    BOOLEAN_TRUE  = 0x01,
}LIBXW_BOOLEAN;


#define _createnode_body(type, nodetype, a) \
{ \
    LIBXW_DATANODE *headnode = NULL; \
    switch (a){ \
    case NODE_HEADNODE_STACK:   \
    case NODE_HEADNODE_QUEUE:   \
    case NODE_HEADNODE_CLINK_COLHEAD:   \
    case NODE_HEADNODE_CLINK_ROWHEAD:   \
    case NODE_HEADNODE_CLINK:   \
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
    if (a == NULL) return LIBXW_ERRNO_NULLOBJECT; \
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

#define _nodescount_body(nodetype, a) \
{ \
    LIBXW_DATANODE *headnode = NULL; \
    int i = 0; \
    if (a != NULL){ \
        headnode = (LIBXW_DATANODE *)a; \
        if ((headnode->datatype & NODE_HEADNODE_##nodetype) > 0){ \
            while (headnode->next != NULL){ \
                headnode = headnode->next; \
                i++; \
            } \
            return i; \
        } \
        else{ \
            return LIBXW_ERRNO_INVALID_NODETYPE; \
        } \
    } \
    else{ \
        return LIBXW_ERRNO_NULLOBJECT; \
    } \
}

#ifdef WIN32
#define _nodescount(type, name, nodetype, atype, a) \
type __cdecl name(atype a) \
_nodescount_body(nodetype, a)
#else
#define _nodescount(type, name, nodetype, atype, a) \
type name(atype a) \
_nodescount_body(nodetype, a)
#endif


#define _nodespeek_body(nodetype, a, b, c, d) \
{ \
    LIBXW_DATANODE *headnode = NULL, *cur = NULL; \
    if (a == NULL) return LIBXW_ERRNO_NULLOBJECT; \
    headnode = (LIBXW_DATANODE *)a; \
    if (((headnode->datatype & NODE_HEADNODE_##nodetype) == 0) || ((headnode->datatype & 0xFF) != b)) \
        return LIBXW_ERRNO_INVALID_NODETYPE; \
    if (TOPPTR_VALUE(headnode) == NULL) return LIBXW_ERRNO_INVALIDOPRATION; \
    cur = TOPPTR_VALUE(headnode); \
    return get_datanode_value(cur, b, c, d); \
}

#ifdef WIN32
#define _nodespeek(type, name, nodetype, atype, a, btype, b, ctype, c, dtype, d) \
type __cdecl name(atype a, btype b, ctype c, dtype d) \
_nodespeek_body(nodetype, a, b, c, d)
#else
#define _nodespeek(type, name, nodetype, atype, a, btype, b, ctype, c, dtype, d) \
type name(atype a, btype b, ctype c, dtype d) \
_nodespeek_body(nodetype, a, b, c, d)
#endif



/* The declearation of the interfaces. */
int convstr(char * str);
int trimstr(char * str, int len);

LIBXW_MANAGED_STACK stack_create(LIBXW_VALUE_TYPE value_type);
int stack_dispose(LIBXW_MANAGED_STACK stack);
int stack_clear(LIBXW_MANAGED_STACK stack);
int stack_count(LIBXW_MANAGED_STACK stack);
int stack_peek(LIBXW_MANAGED_STACK stack, LIBXW_VALUE_TYPE value_type, void * value_ptr, int * value_len_buf);
int stack_pop(LIBXW_MANAGED_STACK stack, LIBXW_VALUE_TYPE value_type, void * value_ptr, int * value_len_buf);
int stack_push(LIBXW_MANAGED_STACK stack, LIBXW_VALUE_TYPE value_type, void * value_ptr, int value_len);

LIBXW_MANAGED_QUEUE queue_create(LIBXW_VALUE_TYPE value_type);
int queue_dispose(LIBXW_MANAGED_QUEUE queue);
int queue_clear(LIBXW_MANAGED_QUEUE queue);
int queue_count(LIBXW_MANAGED_QUEUE queue);
int queue_peek(LIBXW_MANAGED_QUEUE queue, LIBXW_VALUE_TYPE value_type, void * value_ptr, int * value_len_buf);
int queue_enqueue(LIBXW_MANAGED_QUEUE queue, LIBXW_VALUE_TYPE value_type, void * value_ptr, int value_len);
int queue_dequeue(LIBXW_MANAGED_QUEUE queue, LIBXW_VALUE_TYPE value_type, void * value_ptr, int * value_len_buf);

LIBXW_MANAGED_MATRIX matrix_create(LIBXW_VALUE_TYPE value_type, int col_count, int row_count);
int matrix_set_item(LIBXW_MANAGED_MATRIX matrix, LIBXW_VALUE_TYPE value_type, void * value_ptr, int value_len, 
    int column_index, int row_index);
int matrix_get_item(LIBXW_MANAGED_MATRIX matrix, LIBXW_VALUE_TYPE value_type, void * value_ptr, int * value_len_buf, 
    int column_index, int row_index);
int matrix_delete_item(LIBXW_MANAGED_MATRIX matrix, int column_index, int row_index);
int matrix_count_items(LIBXW_MANAGED_MATRIX matrix);
int matrix_resize(LIBXW_MANAGED_MATRIX matrix, int new_cols, int new_rows, LIBXW_BOOLEAN isforced);
int matrix_clear_items(LIBXW_MANAGED_MATRIX matrix);
int matrix_dispose(LIBXW_MANAGED_MATRIX matrix);

#endif
