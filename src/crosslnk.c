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
    int i = 0;

    if (INTEGER_VALUE(matrix_head) == 0){
        return NULL;
    }

    for (cur_ptr = matrix_head, i = -1; i <= column; cur_ptr = cur_ptr->next, i++){
        if (i == column){
            flag_ptr = cur_ptr;
            if (INTEGER_VALUE(flag_ptr) == 0){
                return NULL;
            }

            for (cur_ptr = flag_ptr->prev; cur_ptr != flag_ptr; cur_ptr = cur_ptr->prev){
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
    int i = 0;

    if (matrix == NULL) return LIBXW_ERRNO_NULLOBJECT;

    if (value_ptr == NULL) return LIBXW_ERRNO_NULLARGUMENT;

    if (value_len < 0) return LIBXW_ERRNO_MINUSARGUMENT;

    headnode = (LIBXW_DATANODE *)matrix;

    if ((column < 0) || (column >= headnode->ext.extrec[EXT_COL_INDEX])) return LIBXW_ERRNO_COLINDEX_OUTRANGE;

    if ((row < 0) || (row >= headnode->ext.extrec[EXT_ROW_INDEX])) return LIBXW_ERRNO_ROWINDEX_OUTRANGE;

    if (((headnode->datatype & NODE_HEADNODE_CLINK) == 0) || ((headnode->datatype & 0xFF) != value_type))
        return LIBXW_ERRNO_INVALID_NODETYPE;

    /* First, look up if the specified item existed. */
    if ((newnode = matrix_lookup_item(headnode, column, row)) != NULL){
        /* overwrite the existing node.*/
        return set_datanode_value(newnode, value_type, value_ptr, value_len);
    }
    else{
        newnode = get_next_available_node(GLOBAL_BLOCK_TABLE);
        if (newnode == NULL){
            exit(EXIT_PROCESS_DEBUG_EVENT);
        }
        set_datanode_value(newnode, value_type, value_ptr, value_len);
        newnode->ext.extrec[EXT_COL_INDEX] = column;
        newnode->ext.extrec[EXT_ROW_INDEX] = row;

        for (cur_ptr = headnode, i = -1; i <= column; cur_ptr = cur_ptr->next, i++){
            if (i == column){
                flag_ptr = cur_ptr;
                if (cur_ptr->prev == flag_ptr){          /* no item yet */
                    INTEGER_VALUE(flag_ptr) += 1;
                    newnode->prev = flag_ptr;
                    flag_ptr->prev = newnode;
                }
                else{
                    for (prev_ptr = flag_ptr, cur_ptr = flag_ptr->prev; cur_ptr != flag_ptr; cur_ptr = cur_ptr->prev){

                        if (cur_ptr != flag_ptr){
                            if (cur_ptr->ext.extrec[EXT_ROW_INDEX] > row){
                                INTEGER_VALUE(flag_ptr) += 1;
                                newnode->prev = cur_ptr;
                                prev_ptr->prev = newnode;
                                break;
                            }
                        }
                        prev_ptr = cur_ptr;
                    }

                    if (prev_ptr->prev == cur_ptr){
                        INTEGER_VALUE(flag_ptr) += 1;
                        newnode->prev = cur_ptr;
                        prev_ptr->prev = newnode;
                    }
                }
                break;
            }
        }

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
                    for (prev_ptr = flag_ptr, cur_ptr = flag_ptr->next; cur_ptr != flag_ptr; cur_ptr = cur_ptr->next){
                        if (cur_ptr != flag_ptr){
                            if (cur_ptr->ext.extrec[EXT_COL_INDEX] > column){
                                INTEGER_VALUE(flag_ptr) += 1;
                                newnode->next = cur_ptr;
                                prev_ptr->next = newnode;
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
int matrix_delete_item(LIBXW_MANAGED_MATRIX matrix, int column, int row){
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
    if ((cur_ptr->datatype & 0xFF00) == NODE_HEADNODE_CLINK_ROWHEAD){
        INTEGER_VALUE(cur_ptr) -= 1;
    }
    cur_ptr->next = result_ptr->next;
    result_ptr->next = NULL;

    for (cur_ptr = result_ptr; cur_ptr->prev != result_ptr; cur_ptr = cur_ptr->prev){
        if ((cur_ptr->datatype & 0xFF00) == NODE_HEADNODE_CLINK_COLHEAD){
            INTEGER_VALUE(cur_ptr) -= 1;
        }
    }
    if ((cur_ptr->datatype & 0xFF00) == NODE_HEADNODE_CLINK_COLHEAD){
        INTEGER_VALUE(cur_ptr) -= 1;
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

#ifdef WIN32
int __cdecl matrix_resize(LIBXW_MANAGED_MATRIX matrix, int new_col, int new_row, LIBXW_BOOLEAN isforced){
#else
int matrix_resize(LIBXW_MANAGED_MATRIX matrix, int new_col, int new_row, LIBXW_BOOLEAN isforced){
#endif
    LIBXW_DATANODE *headnode = NULL, *curnode = NULL, *new_headnode = NULL, *prevnode = NULL, 
        *delete_node = NULL, *walk_node = NULL, *reset_node = NULL;
    int col_idx = 0, row_idx = 0;
    LIBXW_VALUE_TYPE value_type;

    if (matrix == NULL) return LIBXW_ERRNO_NULLOBJECT;

    headnode = (LIBXW_DATANODE *)matrix;
    if (!(headnode->datatype & NODE_HEADNODE_CLINK)) return LIBXW_ERRNO_INVALID_NODETYPE;

    if (isforced == BOOLEAN_FALSE){
        for (curnode = headnode->next;
            curnode != headnode;
            curnode = curnode->next){
            if (curnode->ext.extrec[EXT_COL_INDEX] >= new_col){
                if (INTEGER_VALUE(curnode) > 0){
                    return LIBXW_ERRNO_INVALIDOPRATION;
                }
            }
        }

        for (curnode = headnode->prev;
            curnode != headnode;
            curnode = curnode->prev){
            if (curnode->ext.extrec[EXT_ROW_INDEX] >= new_row){
                if (INTEGER_VALUE(curnode) > 0){
                    return LIBXW_ERRNO_INVALIDOPRATION;
                }
            }
        }
    }

    value_type = headnode->datatype ^ NODE_HEADNODE_CLINK;

    if (new_col > headnode->ext.extrec[EXT_COL_INDEX]){
        /* Add new colomns */
        for (curnode = headnode, col_idx = -1; 
            ((curnode->next != headnode) && (col_idx < headnode->ext.extrec[EXT_COL_INDEX]));
            curnode = curnode->next, col_idx++){
            ;;
        }
        if (curnode->ext.extrec[EXT_COL_INDEX] != col_idx){
            exit(EXIT_PROCESS_DEBUG_EVENT);
        }

        for (col_idx += 1; col_idx < new_col; col_idx++){
            new_headnode = get_next_available_node(GLOBAL_BLOCK_TABLE);
            if (new_headnode == NULL){
                exit(EXIT_PROCESS_DEBUG_EVENT);
            }
            new_headnode->datatype = NODE_HEADNODE_CLINK_COLHEAD | value_type;
            new_headnode->ext.extrec[EXT_COL_INDEX] = col_idx;
            INTEGER_VALUE(new_headnode) = 0;

            curnode->next = new_headnode;              /* next pointer means right pointer here */
            new_headnode->next = headnode;
            curnode = curnode->next;                   /* move the current pointer*/
            new_headnode->prev = new_headnode;         /* the down pointer should points to the column head itself during initialization*/
        }
        
    }
    else if (new_col < headnode->ext.extrec[EXT_COL_INDEX]){
        /* Delete existing columns */

        for (prevnode = headnode, curnode = headnode->next;
            curnode != headnode;){
            if (curnode->ext.extrec[EXT_COL_INDEX] >= new_col){
                if (INTEGER_VALUE(curnode) > 0){
                    for (delete_node = curnode, walk_node = curnode->prev; walk_node != curnode; ){
                        delete_node = walk_node;
                        walk_node = walk_node->prev;
                        
                        /* Make sure the relation of the row is alright after the delete_node being deleted. */
                        for (reset_node = delete_node; reset_node->next != delete_node; reset_node = reset_node->next){
                            if ((reset_node->datatype & 0xFF00) == NODE_HEADNODE_CLINK_ROWHEAD){
                                INTEGER_VALUE(reset_node) -= 1;
                            }
                        }
                        if ((reset_node->datatype & 0xFF00) == NODE_HEADNODE_CLINK_ROWHEAD){
                            INTEGER_VALUE(reset_node) -= 1;
                        }

                        reset_node->next = delete_node->next;
                        delete_node->next = NULL;
                        INTEGER_VALUE(headnode) -= 1;
                        put_datanode_into_spare(GLOBAL_BLOCK_TABLE, delete_node);
                    }
                    curnode->prev = curnode;
                }

                delete_node = curnode;
                curnode = curnode->next;
                prevnode->next = curnode;
                put_datanode_into_spare(GLOBAL_BLOCK_TABLE, delete_node);
            }
            else{
                prevnode = curnode;
                curnode = curnode->next;
            }
        }
    }
    else{
        ;;
    }
    headnode->ext.extrec[EXT_COL_INDEX] = new_col;

    if (new_row > headnode->ext.extrec[EXT_ROW_INDEX]){
        /* Add new rows */
        for (curnode = headnode, row_idx = -1;
            ((curnode->prev != headnode) && (row_idx < headnode->ext.extrec[EXT_ROW_INDEX]));
            curnode = curnode->prev, row_idx++){
            ;;
        }
        if (curnode->ext.extrec[EXT_ROW_INDEX] != row_idx){
            exit(EXIT_PROCESS_DEBUG_EVENT);
        }
        for (row_idx += 1; row_idx < new_col; row_idx++){
            new_headnode = get_next_available_node(GLOBAL_BLOCK_TABLE);
            if (new_headnode == NULL){
                exit(EXIT_PROCESS_DEBUG_EVENT);
            }
            new_headnode->datatype = NODE_HEADNODE_CLINK_ROWHEAD | value_type;
            new_headnode->ext.extrec[EXT_ROW_INDEX] = row_idx;
            INTEGER_VALUE(new_headnode) = 0;

            curnode->prev = new_headnode;              /* next pointer means right pointer here */
            new_headnode->prev = headnode;
            curnode = curnode->prev;                   /* move the current pointer*/
            new_headnode->next = new_headnode;         /* the down pointer should points to the column head itself during initialization*/
        }
    }
    else if (new_row < headnode->ext.extrec[EXT_ROW_INDEX]){
        /* Delete existing rows */
        for (prevnode = headnode, curnode = headnode->prev;
            curnode != headnode;){
            if (curnode->ext.extrec[EXT_ROW_INDEX] >= new_row){
                if (INTEGER_VALUE(curnode) > 0){
                    for (delete_node = curnode, walk_node = curnode->next;
                        walk_node != curnode;){
                        delete_node = walk_node;
                        walk_node = walk_node->next;
                        
                        /* Make sure the relation of the columns is alright after the delete_node being deleted. */
                        for (reset_node = delete_node; reset_node->prev != delete_node; reset_node = reset_node->prev){ 
                            if ((reset_node->datatype & 0xFF00) == NODE_HEADNODE_CLINK_COLHEAD){
                                INTEGER_VALUE(reset_node) -= 1;
                            }
                        }
                        if ((reset_node->datatype & 0xFF00) == NODE_HEADNODE_CLINK_COLHEAD){
                            INTEGER_VALUE(reset_node) -= 1;
                        }

                        reset_node->prev = delete_node->prev;
                        delete_node->prev = NULL;
                        INTEGER_VALUE(headnode) -= 1;
                        put_datanode_into_spare(GLOBAL_BLOCK_TABLE, delete_node);
                    }
                    curnode->next = curnode;
                }

                delete_node = curnode;
                curnode = curnode->prev;
                prevnode->prev = curnode;
                put_datanode_into_spare(GLOBAL_BLOCK_TABLE, delete_node);
            }
            else{
                prevnode = curnode;
                curnode = curnode->prev;
            }
        }
    }
    else{
        ;;
    }
    headnode->ext.extrec[EXT_ROW_INDEX] = new_row;

    return EXIT_SUCCESS;
}

#ifdef __cplusplus    /* If used by C++ code, */ 
}
#endif
