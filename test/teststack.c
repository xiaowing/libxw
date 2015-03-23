#include <stdio.h>
#include <stdlib.h>

#include "libxw.h"
#include "CUnit.h"

#define STACK_DEPTH  5
#define THREAD_NUM   256   

#ifdef TEST_DEBUG
int thread_count = 0;
MUTEX_T mutex;
#endif

typedef struct testtype{
    int foo;
    double bar;
} TestType;

void test_stack_basic(void){
    int foo = 12048, bar = 2048;
    int tmp = 0, size = 0;

    LIBXW_MANAGED_STACK stack = NULL;
    stack = stack_create(NODE_VALUE_INTEGER);

    stack_push(stack, NODE_VALUE_INTEGER, &foo, sizeof(int));

    CU_ASSERT_EQUAL(stack_count(stack), 1);

    stack_peek(stack, NODE_VALUE_INTEGER, &tmp, &size);

    CU_ASSERT_EQUAL(tmp, foo);

    stack_push(stack, NODE_VALUE_INTEGER, &bar, sizeof(int));

    stack_peek(stack, NODE_VALUE_INTEGER, &tmp, &size);

    CU_ASSERT_EQUAL(tmp, bar);

    stack_dispose(stack);
}

void test_stacks_in_oneblock(void){
    int i = 0;
    LIBXW_MANAGED_STACK first = NULL, last = NULL, onemore = NULL;
    first = stack_create(NODE_VALUE_INTEGER);
    for (i = 1; i < (DATANODE_BLOCK_LENGTH - 1); i++){
        stack_push(first, NODE_VALUE_INTEGER, &i, sizeof(int));
    }
    last = stack_create(NODE_VALUE_INTEGER);
    onemore = stack_create(NODE_VALUE_INTEGER);

    CU_ASSERT_EQUAL(((char *)last - (char *)first) % (DATANODE_BLOCK_LENGTH - 1), 0);

    stack_dispose(last);
    stack_dispose(first);

    stack_dispose(onemore);
}

#ifdef WIN32
DWORD __stdcall ThreadExec(LPVOID pM){
#endif
    LIBXW_MANAGED_STACK stk = NULL;
    int r = 0, rad = 0, popval = 0, size = 0, current_thread_no = 0;

#ifdef TEST_DEBUG
    Lock_Mutex(&mutex);
    current_thread_no = thread_count++;
    Unlock_Mutex(&mutex);
#endif

    stk = stack_create(NODE_VALUE_INTEGER);
    srand((unsigned)time(NULL));    /* set randam seed */

    for (int i = 0; i < STACK_DEPTH; i++){
        r = rand();
        stack_push(stk, NODE_VALUE_INTEGER, &r, sizeof(int));
#ifdef TEST_DEBUG
        if (current_thread_no == 154){
            printf("Pushing ...... %d.\n", r);
        }
#endif
    }

#ifdef TEST_DEBUG
    if ((current_thread_no == 154) || ((current_thread_no == 206))){
        printf("return value of pop is %d.\n",
            stack_pop(stk, NODE_VALUE_INTEGER, &popval, &size));
    }
    else{
        stack_pop(stk, NODE_VALUE_INTEGER, &popval, &size);
    }
#else
    stack_pop(stk, NODE_VALUE_INTEGER, &popval, &size);
#endif

    CU_ASSERT_EQUAL(popval, r);
#ifdef TEST_DEBUG
    if (popval != r){
        printf("The failed thread No. is %d.\n", current_thread_no);
        do{
            printf("%d poped.\n", popval);
        } while (stack_pop(stk, NODE_VALUE_INTEGER, &popval, &size) >= 0);
    }
#endif

    /* dispose the stack randomly. */
    srand((unsigned)time(NULL));    /* set randam seed */
    if((rad = rand()) % 2 == 0)
        stack_dispose(stk);

    return 0;
}

#ifdef WIN32
DWORD __stdcall ThreadExecSingle(LPVOID pM){
#endif
    LIBXW_MANAGED_STACK stk = NULL;
    int r = 0, popval = 0, size = 0, current_thread_no = 0;
    int *ptr = (int *)pM;
    stk = stack_create(NODE_VALUE_INTEGER);
    srand((unsigned)time(NULL));    /* set randam seed */

    for (int i = 0; i < STACK_DEPTH; i++){
        r = rand();
        stack_push(stk, NODE_VALUE_INTEGER, &r, sizeof(int));
    }


    stack_pop(stk, NODE_VALUE_INTEGER, &popval, &size);

    CU_ASSERT_EQUAL(popval, r);

    if (popval != r)
        printf("Assertion failed. The counter is %d.\n", *ptr);

    /* stack_dispose(stk); */
    *ptr += 1;
    return 0;
}

void test_stacks_multi_threads(void){
#ifdef WIN32
    HANDLE handles[THREAD_NUM];
#endif

#ifdef TEST_DEBUG
    Initialize_Mutex(&mutex);
#endif

    for (int i = 0; i < THREAD_NUM; i++){
#ifdef WIN32
        handles[i] = CreateThread(NULL, 0, ThreadExec, NULL, 0, NULL);
#endif
    }

#ifdef WIN32
    WaitForMultipleObjects(THREAD_NUM, handles, TRUE, INFINITE);
#endif

#ifdef TEST_DEBUG
    Destroy_Mutex(&mutex);
#endif
}

void test_stacks_single_threads(void){
    int c = 0;

    for (int i = 0; i < THREAD_NUM; i++){
        ThreadExecSingle(&c);
    }
}

void test_stack_various_type(void){
    double d = 3.1415926, p_d = 0;
    short s = 8, p_s = 0;
    long l = 214566666666666, p_l = 0;
    int size = 0;
    char buf[64] = {0x00};
    TestType src, dst;
    src.bar = 128.2048;
    src.foo = 256;

    LIBXW_MANAGED_STACK stacks[5];
    stacks[0] = stack_create(NODE_VALUE_SMALLINT);
    stacks[1] = stack_create(NODE_VALUE_LONG);
    stacks[2] = stack_create(NODE_VALUE_DOUBLE);
    stacks[3] = stack_create(NODE_VALUE_CSTRING);
    stacks[4] = stack_create(NODE_VALUE_CUSTOMIZED);

    stack_push(stacks[0], NODE_VALUE_SMALLINT, &s, sizeof(short));
    stack_push(stacks[1], NODE_VALUE_LONG, &l, sizeof(long));
    stack_push(stacks[2], NODE_VALUE_DOUBLE, &d, sizeof(double));
    stack_push(stacks[3], NODE_VALUE_CSTRING, "hello, world\n", strlen("hello, world\n"));
    stack_push(stacks[4], NODE_VALUE_CUSTOMIZED, &src, sizeof(TestType));

    stack_pop(stacks[0], NODE_VALUE_SMALLINT, &p_s, &size);
    CU_ASSERT_EQUAL(p_s, s);
    stack_pop(stacks[1], NODE_VALUE_LONG, &p_l, &size);
    CU_ASSERT_EQUAL(p_l, l);
    stack_pop(stacks[2], NODE_VALUE_DOUBLE, &p_d, &size);
    CU_ASSERT_DOUBLE_EQUAL(p_d, d, 0.000001);
    stack_pop(stacks[3], NODE_VALUE_CSTRING, buf, &size);
    CU_ASSERT_NSTRING_EQUAL(buf, "hello, world\n", size);
    stack_pop(stacks[4], NODE_VALUE_CUSTOMIZED, &dst, &size);
    CU_ASSERT_EQUAL(dst.foo, src.foo);
    CU_ASSERT_DOUBLE_EQUAL(dst.bar, src.bar, 0.1);

    for (int i = 0; i < 5; i++){
        stack_dispose(stacks[i]);
    }

}

static CU_TestInfo testcase[] = {
    { "test_stack_basic:", test_stack_basic },
    { "test_stacks_in_oneblock:", test_stacks_in_oneblock },
    { "test_stacks_multi_threads", test_stacks_multi_threads },
    { "test_stacks_single_threads", test_stacks_single_threads },
    { "test_stack_various_type", test_stack_various_type },
     CU_TEST_INFO_NULL
};

int suite_success_init(void){
    return 0;
}

int suite_success_clean(void){
    return 0;
}

static CU_SuiteInfo suites[] = {
    { "Suite of stack test", suite_success_init, suite_success_clean, NULL, NULL, testcase },
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


