#include <stdio.h>
#include <stdlib.h>

#include "libxw.h"
#include "CUnit.h"

#define QUEUE_LENGTH  5
#ifndef THREAD_NUM
#define THREAD_NUM   256
#endif

extern int suite_success_init(void);
extern int suite_success_clean(void);

void test_queue_basic(void){
    int foo = 12048, bar = 2048;
    int tmp = 0, size = 0;

    LIBXW_MANAGED_QUEUE queue = NULL;
    queue = queue_create(NODE_VALUE_INTEGER);

    queue_enqueue(queue, NODE_VALUE_INTEGER, &foo, sizeof(int));

    CU_ASSERT_EQUAL(queue_count(queue), 1);

    queue_peek(queue, NODE_VALUE_INTEGER, &tmp, &size);

    CU_ASSERT_EQUAL(tmp, foo);

    queue_enqueue(queue, NODE_VALUE_INTEGER, &bar, sizeof(int));

    CU_ASSERT_EQUAL(queue_count(queue), 2);

    queue_dequeue(queue, NODE_VALUE_INTEGER, &tmp, &size);

    CU_ASSERT_EQUAL(queue_count(queue), 1);
    CU_ASSERT_EQUAL(tmp, foo);

    queue_dispose(queue);
}

#ifdef WIN32
static DWORD __stdcall ThreadExec(LPVOID pM){
#else
static void* ThreadExec(void* arg){
#endif
    LIBXW_MANAGED_QUEUE que = NULL;
    int r = 0, first = 0, rad = 0, popval = 0, size = 0, current_thread_no = 0, i = 0;

    que = queue_create(NODE_VALUE_INTEGER);
    srand((unsigned)time(NULL));    /* set randam seed */

    for (i = 0; i < QUEUE_LENGTH; i++){
        r = rand();
        if (i == 0)
            first = r;
        queue_enqueue(que, NODE_VALUE_INTEGER, &r, sizeof(int));
    }

    queue_dequeue(que, NODE_VALUE_INTEGER, &popval, &size);

    CU_ASSERT_EQUAL(popval, first);
    /* dispose the stack randomly. */
    srand((unsigned)time(NULL));    /* set randam seed */
    if ((rad = rand()) % 2 == 0)
        queue_dispose(que);

    return 0;
}

void test_queues_multi_threads(void){
    int i = 0;
#ifdef WIN32
    HANDLE handles[THREAD_NUM];
#else
    pthread_t handles[THREAD_NUM];
#endif

#ifndef WIN32
    pthread_setconcurrency(THREAD_NUM);
#endif   

    for (i = 0; i < THREAD_NUM; i++){
#ifdef WIN32
        handles[i] = CreateThread(NULL, 0, ThreadExec, NULL, 0, NULL);
#else
        pthread_create(&handles[i], NULL, ThreadExec, NULL);
#endif
    }

#ifdef WIN32
    WaitForMultipleObjects(THREAD_NUM, handles, TRUE, INFINITE);
#else
    for (i = 0; i < THREAD_NUM; i++){
        pthread_join(handles[i], NULL);
    }
#endif
}

static CU_TestInfo testcase[] = {
    { "test_queue_basic:", test_queue_basic },
    { "test_queues_multi_threads", test_queues_multi_threads },
    CU_TEST_INFO_NULL
};

static CU_SuiteInfo suites[] = {
    { "Suite of queue test", suite_success_init, suite_success_clean, NULL, NULL, testcase },
    CU_SUITE_INFO_NULL
};

void AddQueueTests(){
    if (NULL != CU_get_registry()){
        if (!CU_is_test_running()){
            if (CUE_SUCCESS != CU_register_suites(suites)){
                exit(EXIT_FAILURE);
            }
        }
    }
}
