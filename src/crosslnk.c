/*
========================================================================================
Name        : crosslnk.c
Author      : Yi Yi
Version     :
Copyright   : Under Apache License 2.0
Description : Implementation of the APIs of the matrix based on a cross linked list.
========================================================================================
*/

#include <stdlib.h>
#include <string.h>

#include "libxw.h"

#ifdef __cplusplus    /* If used by C++ code, */
extern "C" {
#endif

/* Declearation of the functions of vm. */
extern int set_datanode_value(LIBXW_DATANODE *, LIBXW_VALUE_TYPE, void *, int);
extern int get_datanode_value(LIBXW_DATANODE *, LIBXW_VALUE_TYPE, void *, int *);
extern LIBXW_DATANODE* get_next_available_node(LIBXW_DATABLOCK_HEAD *);
extern int put_datanode_into_spare(LIBXW_DATABLOCK_HEAD *, LIBXW_DATANODE *);

/* The global variable */
extern LIBXW_DATABLOCK_HEAD *GLOBAL_BLOCK_TABLE;

#ifndef INTEGER_VALUE
#define INTEGER_VALUE(datanode_ptr)  \
    (*((int *)(datanode_ptr->value)))
#endif

#define EXT_ROW_INDEX    1
#define EXT_COL_INDEX    0

/* clink_head_create() */
_createnode(LIBXW_MANAGED_MATRIX, clink_head_create, CLINK, LIBXW_VALUE_TYPE, value_type)

/* matrix_create() */
#ifdef WIN32
LIBXW_MANAGED_MATRIX __cdecl matrix_create(LIBXW_VALUE_TYPE value_type, int col_count, int row_count){
#else
LIBXW_MANAGED_MATRIX matrix_create(LIBXW_VALUE_TYPE value_type, int col_count, int row_count){
#endif
    LIBXW_DATANODE *clink_head = NULL, *new_headnode = NULL, *prev_ptr = NULL;
    LIBXW_MANAGED_MATRIX matrix_head = NULL;
    int col_idx = 0, row_idx = 0;

    if (col_count <= 0) return NULL;
    if (row_count <= 0) return NULL;

    if ((matrix_head = clink_head_create(value_type)) != NULL){
        clink_head = (LIBXW_DATANODE *)matrix_head;
        clink_head->ext.extrec[EXT_COL_INDEX] = col_count;
        clink_head->ext.extrec[EXT_ROW_INDEX] = row_count;
        INTEGER_VALUE(clink_head) = 0;
    }
    else{
        return NULL;
    }

    /* add the column heads */
    for (prev_ptr = clink_head, col_idx = 0; col_idx < col_count; col_idx++){
        new_headnode = get_next_available_node(GLOBAL_BLOCK_TABLE);
        if (new_headnode == NULL){
            exit(EXIT_PROCESS_DEBUG_EVENT);
        }
        new_headnode->datatype = NODE_HEADNODE_CLINK_COLHEAD | value_type;
        new_headnode->ext.extrec[EXT_COL_INDEX] = col_idx;
        INTEGER_VALUE(new_headnode) = 0;

        prev_ptr->next = new_headnode;              /* next pointer means right pointer here */
        new_headnode->next = clink_head;
        prev_ptr = prev_ptr->next;
        new_headnode->prev = new_headnode;          /* the down pointer should points to the column head itself during initialization*/
    }

    /* add the column heads */
    for (prev_ptr = clink_head, row_idx = 0; row_idx < row_count; row_idx++){
        new_headnode = get_next_available_node(GLOBAL_BLOCK_TABLE);
        if (new_headnode == NULL){
            exit(EXIT_PROCESS_DEBUG_EVENT);
        }
        new_headnode->datatype = NODE_HEADNODE_CLINK_ROWHEAD | value_type;
        new_headnode->ext.extrec[EXT_ROW_INDEX] = row_idx;
        INTEGER_VALUE(new_headnode) = 0;

        prev_ptr->prev = new_headnode;               /* prev pointer means down pointer here */
        new_headnode->prev = clink_head;
        prev_ptr = prev_ptr->prev;
        new_headnode->next = new_headnode;           /* the right pointer should points to the row head itself during initialization*/
    }

    return matrix_head;
}

static LIBXW_DATANODE * matrix_lookup_item(LIBXW_DATANODE * matrix_head, int column, int row){
    LIBXW_DATANODE *cur_ptr = NULL, *flag_ptr = NULL;
    int i = 0, flag_walk = 0;

    if (INTEGER_VALUE(matrix_head) == 0){
        return NULL;
    }

    for (cur_ptr = matrix_head, i = -1; i <= column; cur_ptr = cur_ptr->next, i++){
        if (i == column){
            flag_ptr = cur_ptr;
            if (INTEGER_VALUE(flag_ptr) == 0){
                return NULL;
            }

            for (; cur_ptr != flag_ptr || flag_walk == 0; cur_ptr = cur_ptr->prev){
                if (flag_walk != 1) flag_walk = 1;

                if (cur_ptr != flag_ptr){
                    if (cur_ptr->ext.extrec[EXT_ROW_INDEX] == row){
                        return cur_ptr;
                    }
                }
            }
        }
    }

    return NULL;
}

#ifdef WIN32
int __cdecl matrix_set_item(LIBXW_MANAGED_MATRIX matrix, LIBXW_VALUE_TYPE value_type, void * value_ptr, int value_len, 
    int column, int row){
#else
int matrix_set_item(LIBXW_MANAGED_MATRIX matrix, LIBXW_VALUE_TYPE value_type, void * value_ptr, int value_len,
    int column, int row){
#endif
    LIBXW_DATANODE *headnode = NULL, *newnode = NULL, *prev_ptr = NULL, *cur_ptr = NULL, *flag_ptr = NULL;
    int i = 0, flag_newadd = 0, flag_walk = 0;

    if (matrix == NULL) return LIBXW_ERRNO_NULLOBJECT;

    if (value_ptr == NULL) return LIBXW_ERRNO_NULLARGUMENT;

    if (value_len < 0) return LIBXW_ERRNO_MINUSARGUMENT;

    headnode = (LIBXW_DATANODE *)matrix;

    if ((column < 0) || (column >= headnode->ext.extrec[EXT_COL_INDEX])) return LIBXW_ERRNO_COLINDEX_OUTRANGE;

    if ((row < 0) || (row >= headnode->ext.extrec[EXT_ROW_INDEX])) return LIBXW_ERRNO_ROWINDEX_OUTRANGE;

    if (((headnode->datatype & NODE_HEADNODE_CLINK) == 0) || ((headnode->datatype & 0xFF) != value_type))
        return LIBXW_ERRNO_INVALID_NODETYPE;

    newnode = get_next_available_node(GLOBAL_BLOCK_TABLE);
    if (newnode == NULL){
        exit(EXIT_PROCESS_DEBUG_EVENT);
    }
    set_datanode_value(newnode, value_type, value_ptr, value_len);
    newnode->ext.extrec[EXT_COL_INDEX] = column;
    newnode->ext.extrec[EXT_ROW_INDEX] = row;
    
    /* TODO: refactor the following code to cut the use of the variable flag_newadd */
    for (cur_ptr = headnode, i = -1; i <= column; cur_ptr = cur_ptr->next, i++){
        if (i == column){
            flag_ptr = cur_ptr;
            if (cur_ptr->prev == flag_ptr){          /* no item yet */
                INTEGER_VALUE(flag_ptr) += 1;
                newnode->prev = flag_ptr;
                flag_ptr->prev = newnode;
                flag_newadd = 1;
            }
            else{
                for (prev_ptr = flag_ptr; cur_ptr != flag_ptr || flag_walk == 0; cur_ptr = cur_ptr->prev){
                    if (flag_walk != 1) flag_walk = 1;

                    if (cur_ptr != flag_ptr){
                        if (cur_ptr->ext.extrec[EXT_ROW_INDEX] > row){
                            INTEGER_VALUE(flag_ptr) += 1;
                            newnode->prev = cur_ptr;
                            prev_ptr->prev = newnode;
                            flag_newadd = 1;
                            flag_walk = 0;
                            break;
                        }
                        else if (cur_ptr->ext.extrec[EXT_ROW_INDEX] == row){    /* specified [col, row] already exists. */
                            /* overwrite the existing node.*/
                            return set_datanode_value(cur_ptr, value_type, value_ptr, value_len);
                            /* QUESTION: does the caller of matrix_set_item() need to know whether the overwrite happened ? */
                        }
                        else{
                            ;;
                        }
                    }
                    prev_ptr = cur_ptr;
                }
                flag_walk = 0;

                if (prev_ptr->prev == cur_ptr){
                    INTEGER_VALUE(flag_ptr) += 1;
                    newnode->prev = cur_ptr;
                    prev_ptr->prev = newnode;
                    flag_newadd = 1;
                }
            }
            break;
        }
    }

    if (flag_newadd == 1){       /* in the condition that new node added*/
        INTEGER_VALUE(headnode) += 1;

        for (cur_ptr = headnode, i = -1; i <= row; cur_ptr = cur_ptr->prev, i++){
            if (i == row){
                flag_ptr = cur_ptr;
                if (cur_ptr->next == flag_ptr){          
                    INTEGER_VALUE(flag_ptr) += 1;
                    newnode->next = flag_ptr;
                    flag_ptr->next = newnode;
                }
                else{
                    for (prev_ptr = flag_ptr; cur_ptr != flag_ptr || flag_walk == 0; cur_ptr = cur_ptr->next){
                        if (flag_walk != 1) flag_walk = 1;

                        if (cur_ptr != flag_ptr){
                            if (cur_ptr->ext.extrec[EXT_COL_INDEX] > column){
                                INTEGER_VALUE(flag_ptr) += 1;
                                newnode->next = cur_ptr;
                                prev_ptr->next = newnode;
                                flag_walk = 0;
                                break;
                            }
                            else if (cur_ptr->ext.extrec[EXT_COL_INDEX] == column){
                                return LIBXW_ERRNO_COLINDEX_EXISTED;
                            }
                            else{
                                ;;
                            }
                        }
                        prev_ptr = cur_ptr;
                    }
                    flag_walk = 0;

                    if (prev_ptr->next == cur_ptr){
                        INTEGER_VALUE(flag_ptr) += 1;
                        newnode->next = cur_ptr;
                        prev_ptr->next = newnode;
                    }
                }
                break;
            }
        }

        return EXIT_SUCCESS;
    }
}


#ifdef WIN32
int __cdecl matrix_get_item(LIBXW_MANAGED_MATRIX matrix, LIBXW_VALUE_TYPE value_type, void *value_buf, int *value_len_ptr,
    int column, int row){
#else
int matrix_get_item(LIBXW_MANAGED_MATRIX matrix, LIBXW_VALUE_TYPE value_type, void *value_buf, int *value_len_ptr,
    int column, int row){
#endif
    LIBXW_DATANODE *headnode = NULL, *result_ptr = NULL;

    if (matrix == NULL) return LIBXW_ERRNO_NULLOBJECT;

    if (value_buf == NULL) return LIBXW_ERRNO_NULLARGUMENT;

    headnode = (LIBXW_DATANODE *)matrix;

    if ((column < 0) || (column >= headnode->ext.extrec[EXT_COL_INDEX])) return LIBXW_ERRNO_COLINDEX_OUTRANGE;

    if ((row < 0) || (row >= headnode->ext.extrec[EXT_ROW_INDEX])) return LIBXW_ERRNO_ROWINDEX_OUTRANGE;

    if (((headnode->datatype & NODE_HEADNODE_CLINK) == 0) || ((headnode->datatype & 0xFF) != value_type))
        return LIBXW_ERRNO_INVALID_NODETYPE;

    if ((result_ptr = matrix_lookup_item(headnode, column, row)) == NULL) return LIBXW_ERRNO_NOT_FOUND;

    return get_datanode_value(result_ptr, value_type, value_buf, value_len_ptr);
}

#ifdef WIN32
int __cdecl matrix_delete_item(LIBXW_MANAGED_MATRIX matrix, int column, int row){
#else
int matrix_get_item(LIBXW_MANAGED_MATRIX matrix, int column, int row){
#endif
    LIBXW_DATANODE *headnode = NULL, *result_ptr = NULL, *cur_ptr = NULL;

    if (matrix == NULL) return LIBXW_ERRNO_NULLOBJECT;

    headnode = (LIBXW_DATANODE *)matrix;

    if ((column < 0) || (column >= headnode->ext.extrec[EXT_COL_INDEX])) return LIBXW_ERRNO_COLINDEX_OUTRANGE;

    if ((row < 0) || (row >= headnode->ext.extrec[EXT_ROW_INDEX])) return LIBXW_ERRNO_ROWINDEX_OUTRANGE;

    if ((result_ptr = matrix_lookup_item(headnode, column, row)) == NULL) return LIBXW_ERRNO_NOT_FOUND;

    INTEGER_VALUE(headnode) -= 1;

    for (cur_ptr = result_ptr; cur_ptr->next != result_ptr; cur_ptr = cur_ptr->next){
        if ((cur_ptr->datatype & 0xFF00) == NODE_HEADNODE_CLINK_ROWHEAD){
            INTEGER_VALUE(cur_ptr) -= 1;
        }
    }
    cur_ptr->next = result_ptr->next;
    result_ptr->next = NULL;

    for (cur_ptr = result_ptr; cur_ptr->prev != result_ptr; cur_ptr = cur_ptr->prev){
        if ((cur_ptr->datatype & 0xFF00) == NODE_HEADNODE_CLINK_COLHEAD){
            INTEGER_VALUE(cur_ptr) -= 1;
        }
    }
    cur_ptr->prev = result_ptr->prev;
    result_ptr->prev = NULL;

    put_datanode_into_spare(GLOBAL_BLOCK_TABLE, result_ptr);
    return EXIT_SUCCESS;
}

#ifdef WIN32
int __cdecl matrix_count_items(LIBXW_MANAGED_MATRIX matrix){
#else
int matrix_count_items(LIBXW_MANAGED_MATRIX matrix){
#endif
    LIBXW_DATANODE *headnode = NULL;

    if (matrix == NULL) return LIBXW_ERRNO_NULLOBJECT;

    headnode = (LIBXW_DATANODE *)matrix;

    return INTEGER_VALUE(headnode);
}

/* TODO: matrix_clean_item() and matrix_dispose() */
#ifdef WIN32
int __cdecl matrix_clear_items(LIBXW_MANAGED_MATRIX matrix){
#else
int matrix_clear_items(LIBXW_MANAGED_MATRIX matrix){
#endif
    LIBXW_DATANODE *headnode = NULL, *flag_ptr = NULL, *cur_ptr = NULL, *prev_ptr = NULL;
    int i = 0;

    if (matrix == NULL) return LIBXW_ERRNO_NULLOBJECT;

    headnode = (LIBXW_DATANODE *)matrix;

    if (INTEGER_VALUE(headnode) != 0){
        for (flag_ptr = headnode, i = -1; i < headnode->ext.extrec[EXT_COL_INDEX]; flag_ptr = flag_ptr->next, i++){
            if ((flag_ptr->datatype & 0xFF00) == NODE_HEADNODE_CLINK) continue;

            for (cur_ptr = flag_ptr->prev, prev_ptr = flag_ptr; cur_ptr != flag_ptr; cur_ptr = cur_ptr->prev){
                /* if (cur_ptr->datatype & 0xFF00 == NODE_HEADNODE_CLINK_COLHEAD) {
                    prev_ptr = cur_ptr;
                    continue;
                } */

                if ((prev_ptr->datatype & 0xFF00) != NODE_HEADNODE_CLINK_COLHEAD){
                    put_datanode_into_spare(GLOBAL_BLOCK_TABLE, prev_ptr);
                    
                    /* TODO: Just for test. No need to do this. INTEGER_VALUE(headnode) = 0 is fine */
                    INTEGER_VALUE(headnode) -= 1;    
                }
                prev_ptr = cur_ptr;
            }

            /* An addional clean should be executed after loop because when the loop stops, one last node still exists */ 
            if ((prev_ptr->datatype & 0xFF00) != NODE_HEADNODE_CLINK_COLHEAD){
                put_datanode_into_spare(GLOBAL_BLOCK_TABLE, prev_ptr);
                INTEGER_VALUE(headnode) -= 1;    /* TODO: Just for test. No need to do this */
            }

            /* recovery of the status flag. */
            if ((flag_ptr->datatype & 0xFF00) == NODE_HEADNODE_CLINK_COLHEAD){
                INTEGER_VALUE(flag_ptr) = 0;
                flag_ptr->prev = flag_ptr;
            }
        }

        for (flag_ptr = headnode, i = -1; i < headnode->ext.extrec[EXT_ROW_INDEX]; flag_ptr = flag_ptr->prev, i++){
            if ((flag_ptr->datatype & 0xFF00) == NODE_HEADNODE_CLINK) continue;
            
            if ((flag_ptr->datatype & 0xFF00) == NODE_HEADNODE_CLINK_ROWHEAD){
                INTEGER_VALUE(flag_ptr) = 0;
                flag_ptr->next = flag_ptr;
            }
        }
    }
    return EXIT_SUCCESS;
}

#ifdef WIN32
int __cdecl matrix_dispose(LIBXW_MANAGED_MATRIX matrix){
#else
int matrix_dispose(LIBXW_MANAGED_MATRIX matrix){
#endif
    LIBXW_DATANODE *headnode = NULL, *cur_ptr = NULL, *prev_ptr = NULL;
    int ret = 0;

    ret = matrix_clear_items(matrix);
    if (ret != EXIT_SUCCESS){
        return ret;
    }

    headnode = (LIBXW_DATANODE *)matrix;

    /* clean the column heads */
    for (cur_ptr = headnode->next, prev_ptr = headnode; cur_ptr != headnode; cur_ptr = cur_ptr->next){
        if ((prev_ptr->datatype & 0xFF00) != NODE_HEADNODE_CLINK){
            put_datanode_into_spare(GLOBAL_BLOCK_TABLE, prev_ptr);  
        }
        prev_ptr = cur_ptr;
    }
    if ((prev_ptr->datatype & 0xFF00) != NODE_HEADNODE_CLINK){
        put_datanode_into_spare(GLOBAL_BLOCK_TABLE, prev_ptr);
    }

    /* clean the row heads */
    for (cur_ptr = headnode->prev, prev_ptr = headnode; cur_ptr != headnode; cur_ptr = cur_ptr->prev){
        if ((prev_ptr->datatype & 0xFF00) != NODE_HEADNODE_CLINK){
            put_datanode_into_spare(GLOBAL_BLOCK_TABLE, prev_ptr);
        }
        prev_ptr = cur_ptr;
    }
    if ((prev_ptr->datatype & 0xFF00) != NODE_HEADNODE_CLINK){
        put_datanode_into_spare(GLOBAL_BLOCK_TABLE, prev_ptr);
    }

    /* clean the matrix head */
    put_datanode_into_spare(GLOBAL_BLOCK_TABLE, headnode);

    return EXIT_SUCCESS;
}

#ifdef __cplusplus    /* If used by C++ code, */ 
}
#endif
