#include <stdio.h>
#include <stdlib.h>

#include "libxw.h"
#include "CUnit.h"

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

static CU_TestInfo testcase[] = {
    { "test_queue_basic:", test_queue_basic },
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
