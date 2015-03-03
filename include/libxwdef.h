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
#define DATANODE_BLOCK_LENGTH   256

#define LIBXW_ERRNO_NULLSTACK -1
#define LIBXW_ERRNO_INVALID_NODETYPE -2
#define LIBXW_ERRNO_NULLARGUMENT -3
#define LIBXW_ERRNO_MINUSARGUMENT -4
#define LIBXW_ERRNO_INVALIDOPRATION -5

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

#endif
