#include <stdio.h>
#include <stdlib.h>

#include "libxw.h"
#include "CUnit.h"

void test_stack_basic(void){
    int foo = 12048, bar = 2048;
    int tmp = 0, size = 0;

    LIBXW_MANAGED_STACK *stack = NULL;
    stack = stack_create(NODE_VALUE_INTEGER);

    stack_push(stack, NODE_VALUE_INTEGER, &foo, sizeof(int));

    CU_ASSERT_EQUAL(stack_items_counter(stack), 1);

    stack_peek(stack, NODE_VALUE_INTEGER, &tmp, &size);

    CU_ASSERT_EQUAL(tmp, foo);

    stack_push(stack, NODE_VALUE_INTEGER, &bar, sizeof(int));

    stack_peek(stack, NODE_VALUE_INTEGER, &tmp, &size);

    CU_ASSERT_EQUAL(tmp, bar);

    stack_dispose(stack);
}

CU_TestInfo testcase[] = {
    { "test_stack_basic:", test_stack_basic },
     CU_TEST_INFO_NULL
};

int suite_success_init(void){
    return 0;
}

int suite_success_clean(void){
    return 0;
}

CU_SuiteInfo suites[] = {
    { "testSuite1", suite_success_init, suite_success_clean, NULL, NULL, testcase },
    CU_SUITE_INFO_NULL
};


void AddStackTests(){
    if (NULL != CU_get_registry()){
        if (!CU_is_test_running()){
            if (CUE_SUCCESS != CU_register_suites(suites)){
                exit(EXIT_FAILURE);
            }
        }
    }
}


