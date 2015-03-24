/*
============================================================================
Name        : stqu.c
Author      : Yi Yi
Version     :
Copyright   : Under Apache License 2.0
Description : Implementation of the APIs of the stack and queue
============================================================================
*/

#include <stdlib.h>
#include <string.h>

#include "libxw.h"

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {
#endif

/* Shortcut macro functions. */
#define TOPPTR_VALUE(datanode_ptr)  \
    (*((LIBXW_DATANODE **)(datanode_ptr->value)))



/* Declearation of the functions of vm. */
extern LIBXW_DATANODE* remove_last_node_from_list(LIBXW_DATANODE*);
extern LIBXW_DATANODE* remove_current_node_from_list(LIBXW_DATANODE*);
extern void put_node_into_rear(LIBXW_DATANODE*, LIBXW_DATANODE*);
extern int set_datanode_value(LIBXW_DATANODE *, LIBXW_VALUE_TYPE, void *, int);
extern int get_datanode_value(LIBXW_DATANODE *, LIBXW_VALUE_TYPE, void *, int *);
extern LIBXW_DATANODE* get_next_available_node(LIBXW_DATABLOCK_HEAD *);
extern int put_datanode_into_spare(LIBXW_DATABLOCK_HEAD *, LIBXW_DATANODE *);



/* The global variable */
extern LIBXW_DATABLOCK_HEAD *GLOBAL_BLOCK_TABLE;

/* The definition of external interface. */
/* stack_create() */
_createnode(LIBXW_MANAGED_STACK, stack_create, STACK, LIBXW_VALUE_TYPE, value_type)

/* stack_clear() */
_clearnodes(int, stack_clear, STACK, LIBXW_MANAGED_STACK, stack)

/* stack_dispose() */
_diposehead(int, stack_dispose, LIBXW_MANAGED_STACK, stack)

/* stack_count() */
_nodescount(int, stack_count, STACK, LIBXW_MANAGED_STACK, stack)

/* stack_peek() */
_nodespeek(int, stack_peek, STACK, LIBXW_MANAGED_STACK, stack, LIBXW_VALUE_TYPE, value_type, void*, value_buf, int*, value_len_ptr)

/* queue_create() */
_createnode(LIBXW_MANAGED_QUEUE, queue_create, QUEUE, LIBXW_VALUE_TYPE, value_type)

/* queue_clear() */
_clearnodes(int, queue_clear, QUEUE, LIBXW_MANAGED_QUEUE, queue)

/* queue_dispose() */
_diposehead(int, queue_dispose, LIBXW_MANAGED_QUEUE, queue)

/* queue_count() */
_nodescount(int, queue_count, QUEUE, LIBXW_MANAGED_QUEUE, queue)

/* queue_peek() */
_nodespeek(int, queue_peek, QUEUE, LIBXW_MANAGED_QUEUE, queue, LIBXW_VALUE_TYPE, value_type, void*, value_buf, int*, value_len_ptr)

/* stack_push() */
#ifdef WIN32
int __cdecl stack_push(LIBXW_MANAGED_STACK stack, LIBXW_VALUE_TYPE value_type, void * value_ptr, int value_len){
#else
int stack_push(LIBXW_MANAGED_STACK stack, LIBXW_VALUE_TYPE value_type, void * value_ptr, int value_len){
#endif
    LIBXW_DATANODE *headnode = NULL, *newnode = NULL;

    if (stack == NULL) return LIBXW_ERRNO_NULLOBJECT;

    if (value_ptr == NULL) return LIBXW_ERRNO_NULLARGUMENT;

    if (value_len < 0) return LIBXW_ERRNO_MINUSARGUMENT;

    headnode = (LIBXW_DATANODE *)stack;

    if (((headnode->datatype & NODE_HEADNODE_STACK) == 0) || ((headnode->datatype & 0xFF) != value_type))
        return LIBXW_ERRNO_INVALID_NODETYPE;

    newnode = get_next_available_node(GLOBAL_BLOCK_TABLE);
    if (newnode == NULL){
        exit(EXIT_PROCESS_DEBUG_EVENT);
    }

    set_datanode_value(newnode, value_type, value_ptr, value_len);

    if (headnode->next != NULL){
        put_node_into_rear(headnode->next, newnode);
    }
    else{
        headnode->next = newnode;
        headnode->prev = NULL;
        newnode->prev = headnode;
    }
    TOPPTR_VALUE(headnode) = newnode;

    return EXIT_SUCCESS;
}

#ifdef WIN32
int __cdecl stack_pop(LIBXW_MANAGED_STACK stack, LIBXW_VALUE_TYPE value_type, void *value_buf, int *value_len_ptr){
#else
int stack_pop(LIBXW_MANAGED_STACK stack, LIBXW_VALUE_TYPE value_type, void *value_buf, int *value_len_ptr){
#endif
    LIBXW_DATANODE *headnode = NULL, *popnode = NULL, *cur = NULL;
    int ret = 0;

    if (stack == NULL) return LIBXW_ERRNO_NULLOBJECT;

    headnode = (LIBXW_DATANODE *)stack;

    if (((headnode->datatype & NODE_HEADNODE_STACK) == 0) || ((headnode->datatype & 0xFF) != value_type))
        return LIBXW_ERRNO_INVALID_NODETYPE;

    if (headnode->next != NULL){
        popnode = remove_last_node_from_list(headnode->next);

        if (popnode == headnode->next){
            headnode->next = NULL;
        }

        cur = headnode;
        /* TODO: FIX IT!!*/
        while (cur->next != NULL){
            cur = cur->next;
        }
        if (cur != headnode){
            TOPPTR_VALUE(headnode) = cur;
        }

        ret = get_datanode_value(popnode, value_type, value_buf, value_len_ptr);

        put_datanode_into_spare(GLOBAL_BLOCK_TABLE, popnode);

        return ret;
    }
    else{
        TOPPTR_VALUE(headnode) = NULL;
        return LIBXW_ERRNO_INVALIDOPRATION;
    }
}

/*
#ifdef WIN32
int __cdecl stack_peek(LIBXW_MANAGED_STACK stack, LIBXW_VALUE_TYPE value_type, void *value_buf, int *value_len_ptr){
#else
int stack_peek(LIBXW_MANAGED_STACK stack, LIBXW_VALUE_TYPE value_type, void *value_buf, int *value_len_ptr){
#endif
    LIBXW_DATANODE *headnode = NULL, *cur = NULL;

    if (stack == NULL) return LIBXW_ERRNO_NULLOBJECT;

    headnode = (LIBXW_DATANODE *)stack;

    if (((headnode->datatype & NODE_HEADNODE_STACK) == 0) || ((headnode->datatype & 0xFF) != value_type))
        return LIBXW_ERRNO_INVALID_NODETYPE;

    if (stack_count(stack) <= 0){
        return LIBXW_ERRNO_INVALIDOPRATION;
    }

    cur = TOPPTR_VALUE(headnode);
    return get_datanode_value(cur, value_type, value_buf, value_len_ptr);
}
*/

/* queue_enqueue() */
#ifdef WIN32
int __cdecl queue_enqueue(LIBXW_MANAGED_QUEUE queue, LIBXW_VALUE_TYPE value_type, void * value_ptr, int value_len){
#else
int queue_enqueue(LIBXW_MANAGED_QUEUE queue, LIBXW_VALUE_TYPE value_type, void * value_ptr, int value_len){
#endif
    LIBXW_DATANODE *headnode = NULL, *newnode = NULL;

    if (queue == NULL) return LIBXW_ERRNO_NULLOBJECT;

    if (value_ptr == NULL) return LIBXW_ERRNO_NULLARGUMENT;

    if (value_len < 0) return LIBXW_ERRNO_MINUSARGUMENT;

    headnode = (LIBXW_DATANODE *)queue;

    if (((headnode->datatype & NODE_HEADNODE_QUEUE) == 0) || ((headnode->datatype & 0xFF) != value_type))
        return LIBXW_ERRNO_INVALID_NODETYPE;

    newnode = get_next_available_node(GLOBAL_BLOCK_TABLE);
    if (newnode == NULL){
        exit(EXIT_PROCESS_DEBUG_EVENT);
    }

    set_datanode_value(newnode, value_type, value_ptr, value_len);

    if (headnode->next != NULL){
        put_node_into_rear(headnode->next, newnode);
    }
    else{
        headnode->next = newnode;
        headnode->prev = NULL;
        newnode->prev = headnode;
        TOPPTR_VALUE(headnode) = headnode->next;
    }

    return EXIT_SUCCESS;
}

/* queue_dequeue() */
#ifdef WIN32
int __cdecl queue_dequeue(LIBXW_MANAGED_QUEUE queue, LIBXW_VALUE_TYPE value_type, void *value_buf, int *value_len_ptr){
#else
int queue_dequeue(LIBXW_MANAGED_QUEUE queue, LIBXW_VALUE_TYPE value_type, void *value_buf, int *value_len_ptr){
#endif
    LIBXW_DATANODE *headnode = NULL, *popnode = NULL, *cur = NULL;
    int ret = 0;

    if (queue == NULL) return LIBXW_ERRNO_NULLOBJECT;

    headnode = (LIBXW_DATANODE *)queue;

    if (((headnode->datatype & NODE_HEADNODE_QUEUE) == 0) || ((headnode->datatype & 0xFF) != value_type))
        return LIBXW_ERRNO_INVALID_NODETYPE;

    if (TOPPTR_VALUE(headnode) != NULL){
        popnode = remove_current_node_from_list(TOPPTR_VALUE(headnode));
        TOPPTR_VALUE(headnode) = headnode->next;

        ret = get_datanode_value(popnode, value_type, value_buf, value_len_ptr);

        put_datanode_into_spare(GLOBAL_BLOCK_TABLE, popnode);

        return ret;
    }
    else{
        return LIBXW_ERRNO_INVALIDOPRATION;
    }
}


#ifdef __cplusplus    // If used by C++ code, 
}
#endif