/*
============================================================================
Name        : minivm.c
Author      : Yi Yi
Version     :
Copyright   : Under Apache License 2.0
Description : Implementation of a minium vm to manage the stack and queue
============================================================================
*/

#include <stdlib.h>
#include <string.h>

#include "libxw.h"

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {
#endif

/* Shortcut macro functions. */
/* #define TOPPTR_VALUE(datanode_ptr)  \
    (*((LIBXW_DATANODE **)(datanode_ptr->value))) */

#define CSTRING_VALUE(datanode_ptr)  \
    (*((char **)(datanode_ptr->value)))

#define CUSTRUCT_VALUE(datanode_ptr)  \
    (*((void **)(datanode_ptr->value)))

#define CHAR_VALUE(datanode_ptr)  \
    (datanode_ptr->value[0])

#define INTEGER_VALUE(datanode_ptr)  \
    (*((int *)(datanode_ptr->value)))

#define SHORTINT_VALUE(datanode_ptr)  \
    (*((short *)(datanode_ptr->value)))

#define LONGINT_VALUE(datanode_ptr)  \
    (*((long *)(datanode_ptr->value)))

#define FLOAT_VALUE(datanode_ptr)  \
    (*((float *)(datanode_ptr->value)))

#define DOUBLE_VALUE(datanode_ptr)  \
    (*((double *)(datanode_ptr->value)))

#define SPARE_NODE_DATA(datanode_ptr)  \
    memset(datanode_ptr, 0x00, sizeof(LIBXW_DATANODE)); \
    datanode_ptr->datatype = NODE_DATANODE_SPARE;

/* The global memory management table. */
LIBXW_DATABLOCK_HEAD *GLOBAL_BLOCK_TABLE = NULL;

/* The critical section for controlling the multi-threaded access of the global resource*/
MUTEX_T   mutex_lock;

/* The internal functions of minivm. */
static LIBXW_DATABLOCK* initial_datablock(void){
    LIBXW_DATABLOCK* result = malloc(sizeof(LIBXW_DATABLOCK));
    if (result == NULL){
        /* TODO: write the syslog */
        exit(EXIT_FAILURE);
    }

    memset((char *)result, 0x00, sizeof(LIBXW_DATABLOCK));

    return result;
}

LIBXW_DATANODE* remove_last_node_from_list(LIBXW_DATANODE* list){
    LIBXW_DATANODE * last = NULL;
    LIBXW_DATANODE * pre = NULL;

    if (list != NULL){
        last = list;
        while (last->next != NULL){
            last = last->next;
        }

        if (last->prev != NULL){
            pre = last->prev;

            pre->next = NULL;
            last->prev = NULL;
        }

        return last;
    }
    else{
        return NULL;
    }
}

LIBXW_DATANODE* remove_current_node_from_list(LIBXW_DATANODE* cur){
    LIBXW_DATANODE * prv = NULL;

    if (cur != NULL){
        prv = cur->prev;
        if (prv != NULL){
            prv->next = cur->next;
            if (cur->next != NULL){
                cur->next->prev = prv;
            }
        }
        else{
            if (cur->next != NULL){
                cur->next->prev = NULL;
            }
        }
        cur->next = NULL;
        cur->prev = NULL;
    }
    return cur;
}

void put_node_into_rear(LIBXW_DATANODE* list, LIBXW_DATANODE* target){
    LIBXW_DATANODE * last = NULL;

     if (list != NULL){
        last = list;
        while (last->next != NULL){
            last = last->next;
        }

        last->next = target;
        target->next = NULL;
        target->prev = last;
    }
}

/*static void put_node_into_front(LIBXW_DATANODE* list, LIBXW_DATANODE* target){
    if (list != NULL){
        target->next = list;
        list->prev = target;
    }
}*/

int set_datanode_value(LIBXW_DATANODE *newnode, LIBXW_VALUE_TYPE value_type, void *value_ptr, int value_len){
    switch (value_type){
    case NODE_VALUE_CHAR:
        CHAR_VALUE(newnode) = *((char *)value_ptr);
        newnode->valuelen = sizeof(char);
        break;
    case NODE_VALUE_INTEGER:
        INTEGER_VALUE(newnode) = *((int *)value_ptr);
        newnode->valuelen = sizeof(int);
        break;
    case NODE_VALUE_SMALLINT:
        SHORTINT_VALUE(newnode) = *((short *)value_ptr);
        newnode->valuelen = sizeof(short);
        break;
    case NODE_VALUE_LONG:
        LONGINT_VALUE(newnode) = *((long *)value_ptr);
        newnode->valuelen = sizeof(long);
        break;
    case NODE_VALUE_FLOAT:
        FLOAT_VALUE(newnode) = *((float *)value_ptr);
        newnode->valuelen = sizeof(float);
        break;
    case NODE_VALUE_DOUBLE:
        DOUBLE_VALUE(newnode) = *((double *)value_ptr);
        newnode->valuelen = sizeof(double);
        break;
    case NODE_VALUE_CSTRING:
        CSTRING_VALUE(newnode) = (char *)value_ptr;
        newnode->valuelen = value_len;
        break;
    case NODE_VALUE_CUSTOMIZED:
        CUSTRUCT_VALUE(newnode) = value_ptr;
        newnode->valuelen = value_len;
        break;
    default:
        return LIBXW_ERRNO_INVALID_NODETYPE;
    }

    newnode->datatype = value_type;
    return EXIT_SUCCESS;
}

int get_datanode_value(LIBXW_DATANODE *node, LIBXW_VALUE_TYPE value_type, void * value_buf, int *value_len_ptr){
    switch (value_type){
    case NODE_VALUE_CHAR:
        *((char *)value_buf) = CHAR_VALUE(node);
        break;
    case NODE_VALUE_INTEGER:
        *((int *)value_buf) = INTEGER_VALUE(node);
        break;
    case NODE_VALUE_SMALLINT:
        *((short *)value_buf) = SHORTINT_VALUE(node);
        break;
    case NODE_VALUE_LONG:
        *((long *)value_buf) = LONGINT_VALUE(node);
        break;
    case NODE_VALUE_FLOAT:
        *((float *)value_buf) = FLOAT_VALUE(node);
        break;
    case NODE_VALUE_DOUBLE:
        *((double *)value_buf) = DOUBLE_VALUE(node);
        break;
    case NODE_VALUE_CSTRING:
        strncpy((char *)value_buf, CSTRING_VALUE(node), node->valuelen);
        break;
    case NODE_VALUE_CUSTOMIZED:
        memcpy(value_buf, CUSTRUCT_VALUE(node), node->valuelen);
        break;
    default:
        return LIBXW_ERRNO_INVALID_NODETYPE;
    }
    *value_len_ptr = node->valuelen;
    return EXIT_SUCCESS;
}

LIBXW_DATANODE* get_next_available_node(LIBXW_DATABLOCK_HEAD *table){
    LIBXW_DATANODE * avail = NULL;
    LIBXW_DATABLOCK * newblock = NULL;

    if (table == NULL) return NULL;

    Lock_Mutex(&mutex_lock);

    if (table->spare != NULL){
        avail = remove_last_node_from_list(table->spare);

        /* the spare pointer should be reset to NULL only if 
           the last spare node is retrived to avoid infinite loop */
        if (avail == table->spare){
            table->spare = NULL;
        }

        Unlock_Mutex(&mutex_lock);

        if (avail->datatype == NODE_DATANODE_SPARE){
            /* memset((char *)avail, 0x00, sizeof(LIBXW_DATANODE)); */
            /* no need to memset, because all the field, except the datatype, 
               will be zero after the node retrieved from spare list. */
            avail->datatype = 0x00;
            return avail;
        }
        else{
            /* Internal error. */
            exit(EXIT_PROCESS_DEBUG_EVENT);
        }
    }
    else if (table->current_block != NULL){
        if (table->current_node_index < DATANODE_BLOCK_LENGTH){
            avail = &(table->current_block->nodearray[table->current_node_index]);
            table->current_node_index += 1;
        }
        else if (table->current_node_index == DATANODE_BLOCK_LENGTH){
            newblock = initial_datablock();
            table->current_block->next = newblock;
            table->current_block = newblock;
            avail = &(table->current_block->nodearray[0]);
            table->current_node_index = 1;
        }
        else{
            Unlock_Mutex(&mutex_lock);
            exit(EXIT_PROCESS_DEBUG_EVENT);
        }

        Unlock_Mutex(&mutex_lock);
        return avail;
    }
    else{
        if (table->next == NULL){
            newblock = initial_datablock();
            table->next = newblock;
            table->current_block = newblock;
            avail = &(table->current_block->nodearray[0]);
            table->current_node_index = 1;
            Unlock_Mutex(&mutex_lock);
            return avail;
        }
        else{
            Unlock_Mutex(&mutex_lock);
            exit(EXIT_PROCESS_DEBUG_EVENT);
        }
    }
}

int put_datanode_into_spare(LIBXW_DATABLOCK_HEAD *table, LIBXW_DATANODE *sparenode){
    if (table == NULL) return LIBXW_ERRNO_NULLARGUMENT;
    if (sparenode == NULL) return LIBXW_ERRNO_NULLARGUMENT;

    SPARE_NODE_DATA(sparenode);

    Lock_Mutex(&mutex_lock);

    if (table->spare != NULL){
        put_node_into_rear(table->spare, sparenode);
    }
    else{
        table->spare = sparenode;
        table->spare->next = NULL;
        table->spare->prev = NULL;
    }

    Unlock_Mutex(&mutex_lock);

    return EXIT_SUCCESS;
}



#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE module_handle, DWORD reason_for_call, LPVOID reserved){
    LIBXW_DATABLOCK *dyn_block_list = NULL, *current_free = NULL;

    if (reason_for_call == DLL_PROCESS_ATTACH){
#else
/* the gcc attribute(constructor) declearation goes here */
__attribute__((constructor)) void initializer(void){
#endif
        if (GLOBAL_BLOCK_TABLE == NULL){
            GLOBAL_BLOCK_TABLE = malloc(sizeof(LIBXW_DATABLOCK_HEAD) + sizeof(LIBXW_DATABLOCK));
            if (GLOBAL_BLOCK_TABLE == NULL){
#ifdef WIN32
                return FALSE;
#endif
            }

            memset((char *)GLOBAL_BLOCK_TABLE, 0x00, (sizeof(LIBXW_DATABLOCK_HEAD) + sizeof(LIBXW_DATABLOCK)));

            GLOBAL_BLOCK_TABLE->current_block = (LIBXW_DATABLOCK *)((char *)GLOBAL_BLOCK_TABLE + sizeof(LIBXW_DATABLOCK_HEAD));
            GLOBAL_BLOCK_TABLE->next = GLOBAL_BLOCK_TABLE->current_block;
        }

        Initialize_Mutex(&mutex_lock);

#ifdef WIN32
        return TRUE;
#endif
    }

#ifdef WIN32
    if (reason_for_call == DLL_PROCESS_DETACH){
#else
/* the gcc attribute(destructor) declearation goes here */
__attribute__((destructor)) void finisher(void){
        LIBXW_DATABLOCK *dyn_block_list = NULL, *current_free = NULL;
#endif
        if (GLOBAL_BLOCK_TABLE != NULL){
            dyn_block_list = GLOBAL_BLOCK_TABLE->next->next;
            while (dyn_block_list != NULL){
                current_free = dyn_block_list;
                dyn_block_list = dyn_block_list->next;
                free(current_free);
            }

            free(GLOBAL_BLOCK_TABLE);
            GLOBAL_BLOCK_TABLE = NULL;
        }

        Destroy_Mutex(&mutex_lock);

#ifdef WIN32
        return TRUE;
#endif
    }

#ifdef WIN32
}
#endif

#ifdef __cplusplus    // If used by C++ code, 
}
#endif
