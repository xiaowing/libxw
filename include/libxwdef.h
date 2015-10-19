/*
============================================================================
Name        : libxwdef.h
Author      : Yi Yi
Version     :
Copyright   : Under Apache License 2.0
Description : The header of the general definition of macros used in libxw.
============================================================================
*/

#ifndef LIBXWDEF_HERDER_
#define LIBXWDEF_HERDER_

#define BUF_SIZE   256
#define VALUE_LENGTH    8
#define DUMMY_LENGTH    8
#define SHORT_ARRAY_LEN 2
#define DATANODE_BLOCK_LENGTH   256
#ifdef WIN32
#define FIELD_LENGTH    _MAX_PATH
#else
#define FIELD_LENGTH    PATH_MAX
#endif

#define LIBXW_ERRNO_NULLOBJECT -1
#define LIBXW_ERRNO_INVALID_NODETYPE -2
#define LIBXW_ERRNO_NULLARGUMENT -3
#define LIBXW_ERRNO_MINUSARGUMENT -4
#define LIBXW_ERRNO_INVALIDOPRATION -5
#define LIBXW_ERRNO_COLINDEX_OUTRANGE -6
#define LIBXW_ERRNO_ROWINDEX_OUTRANGE -7
#define LIBXW_ERRNO_COLINDEX_EXISTED  -8
#define LIBXW_ERRNO_NOT_FOUND         -10

#ifndef WIN32
#define EXIT_PROCESS_DEBUG_EVENT    5
#endif

#ifdef WIN32
typedef CRITICAL_SECTION  MUTEX_T, *MUTEX_P_PTR;
#define Initialize_Mutex(mutex_ref)     InitializeCriticalSection(mutex_ref)
#define Lock_Mutex(mutex_ref)           EnterCriticalSection(mutex_ref)
#define Unlock_Mutex(mutex_ref)         LeaveCriticalSection(mutex_ref)
#define Destroy_Mutex(mutex_ref)        DeleteCriticalSection(mutex_ref)
#else
typedef pthread_mutex_t   MUTEX_T, *MUTEX_P_PTR;
#define Initialize_Mutex(mutex_ref)     pthread_mutex_init(mutex_ref, NULL)
#define Lock_Mutex(mutex_ref)           pthread_mutex_lock(mutex_ref)
#define Unlock_Mutex(mutex_ref)         pthread_mutex_unlock(mutex_ref)
#define Destroy_Mutex(mutex_ref)        pthread_mutex_destroy(mutex_ref)
#endif

/* Internal definition of datastructures used in memory pool. */
typedef struct datanode{
    int datatype;
    char value[VALUE_LENGTH];                       /* The value will be the top pointer of
                                                    the stack or queue, if the node is the head node */
    int valuelen;
    union {
        char extval[VALUE_LENGTH];
        short extrec[SHORT_ARRAY_LEN];              /* extrec[0] for colomn, extrec[1] for row. */
    }ext;
    struct datanode *prev;                          /* down pointer in matrix */
    struct datanode *next;                          /* right pointer in matrix */
}LIBXW_DATANODE;

typedef struct datablock{
    LIBXW_DATANODE nodearray[DATANODE_BLOCK_LENGTH];
    struct datablock *next;
}LIBXW_DATABLOCK;

typedef struct datablock_head{
    LIBXW_DATANODE *spare;
    LIBXW_DATABLOCK *current_block;
    int current_node_index;
    LIBXW_DATABLOCK *next;
    char ext_field[FIELD_LENGTH];
}LIBXW_DATABLOCK_HEAD;

#endif
