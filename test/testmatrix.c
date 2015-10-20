#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libxw.h"
#include "CUnit.h"

#define QUEUE_LENGTH  5
#ifndef THREAD_NUM
#define THREAD_NUM   256
#endif

extern int suite_success_init(void);
extern int suite_success_clean(void);

#ifdef WIN32
extern void test_multi_threads(LPTHREAD_START_ROUTINE);
#else
extern void test_multi_threads(single_thread_ptr);
#endif

void test_matrix_basic(void){
    int foo = 12048, bar = 2048;
    int tmp = 0, size = 0;
    char buf[32] = {0x00};
    char * string1 = "this is matrix[0, 0].\n";
    char * string2 = "[1, 1], done.\n";
    char * string3 = "hello, [2, 0].\n";
    char * string4 = "[3, 1] get ready.\n";
    char * string5 = "acknowledge, [2, 4].\n";
    char * string6 = "weclome to [4, 4].\n";
    char * string7 = "there may be a bug. [2, 2].\n";
    char * string8 = "a bug may lie here. [2, 3].\n";
    char * string9 = "alternated string [2, 2].\n";


    LIBXW_MANAGED_MATRIX matrix = NULL, matrix2 = NULL, matrix3 = NULL;
    matrix = matrix_create(NODE_VALUE_INTEGER, 3, 2);
    CU_ASSERT_PTR_NOT_NULL(matrix);

    matrix_set_item(matrix, NODE_VALUE_INTEGER, &foo, sizeof(int), 1, 0);
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 1);

    matrix_set_item(matrix, NODE_VALUE_INTEGER, &bar, sizeof(int), 2, 1);
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 2);

    matrix_get_item(matrix, NODE_VALUE_INTEGER, &tmp, &size, 1, 0);
    CU_ASSERT_EQUAL(tmp, foo);

    CU_ASSERT_EQUAL(matrix_set_item(matrix, NODE_VALUE_INTEGER, &foo, sizeof(int), 3, 0), LIBXW_ERRNO_COLINDEX_OUTRANGE);

    matrix_delete_item(matrix, 2, 1);
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 1);
    CU_ASSERT_EQUAL(matrix_get_item(matrix, NODE_VALUE_INTEGER, &tmp, &size, 2, 1), LIBXW_ERRNO_NOT_FOUND);

    matrix2 = matrix_create(NODE_VALUE_CSTRING, 5, 5);
    CU_ASSERT_PTR_NOT_NULL(matrix2);

    matrix_set_item(matrix2, NODE_VALUE_CSTRING, string1, strlen(string1), 0, 0);
    matrix_set_item(matrix2, NODE_VALUE_CSTRING, string2, strlen(string2), 1, 1);
    matrix_set_item(matrix2, NODE_VALUE_CSTRING, string3, strlen(string3), 2, 0);
    matrix_set_item(matrix2, NODE_VALUE_CSTRING, string4, strlen(string4), 3, 1);
    matrix_set_item(matrix2, NODE_VALUE_CSTRING, string5, strlen(string5), 2, 4);
    matrix_set_item(matrix2, NODE_VALUE_CSTRING, string6, strlen(string6), 4, 4);
    matrix_set_item(matrix2, NODE_VALUE_CSTRING, string7, strlen(string7), 2, 2);
    matrix_set_item(matrix2, NODE_VALUE_CSTRING, string8, strlen(string8), 2, 3);
    matrix_set_item(matrix2, NODE_VALUE_CSTRING, string9, strlen(string9), 2, 2);


    CU_ASSERT_EQUAL(matrix_count_items(matrix2), 8);
    matrix_get_item(matrix2, NODE_VALUE_CSTRING, &buf, &size, 0, 0);
    CU_ASSERT_NSTRING_EQUAL(buf, string1, size);
    memset(buf, 0x00, sizeof(buf));
    matrix_delete_item(matrix2, 1, 1);
    matrix_get_item(matrix2, NODE_VALUE_CSTRING, &buf, &size, 3, 1);
    CU_ASSERT_NSTRING_EQUAL(buf, string4, size);
    memset(buf, 0x00, sizeof(buf));
    matrix_get_item(matrix2, NODE_VALUE_CSTRING, &buf, &size, 2, 2);
    CU_ASSERT_NSTRING_EQUAL(buf, string9, size);
    CU_ASSERT_EQUAL(matrix_count_items(matrix2), 7);

    CU_ASSERT_EQUAL(matrix_set_item(matrix2, NODE_VALUE_CSTRING, string6, strlen(string6), 5, 5),
        LIBXW_ERRNO_COLINDEX_OUTRANGE);

    /* add the tests of matrix_delete_item() */
    matrix_delete_item(matrix2, 0, 0);
    CU_ASSERT_EQUAL(matrix_count_items(matrix2), 6);
    matrix_delete_item(matrix2, 4, 4);
    CU_ASSERT_EQUAL(matrix_count_items(matrix2), 5);
    matrix_delete_item(matrix2, 3, 1);
    CU_ASSERT_EQUAL(matrix_count_items(matrix2), 4);



    matrix_clear_items(matrix2);
    CU_ASSERT_EQUAL(matrix_count_items(matrix2), 0);
    matrix_dispose(matrix2);

    matrix3 = matrix_create(NODE_VALUE_INTEGER, 2, 1);
    CU_ASSERT_PTR_EQUAL(matrix3, matrix2);
    matrix_set_item(matrix3, NODE_VALUE_INTEGER, &foo, sizeof(int), 0, 0);
    matrix_set_item(matrix3, NODE_VALUE_INTEGER, &bar, sizeof(int), 1, 0);
    CU_ASSERT_EQUAL(matrix_count_items(matrix3), 2);
    matrix_dispose(matrix3);
}

#ifdef WIN32
static DWORD __stdcall ThreadExec(LPVOID pM){
#else
static void* ThreadExec(void* arg){
#endif
    LIBXW_MANAGED_QUEUE matrix = NULL;
    int r = 0, mod = 0, rad = 0, popval = 0, size = 0, col = 0, row = 0, i = 0, j = 0;
    int indexs[QUEUE_LENGTH][3] = { 0 };

    srand((unsigned)time(NULL));
    do{
        mod = rand() % 10;
    } while (mod < 3);
    matrix = matrix_create(NODE_VALUE_INTEGER, mod, mod);

    srand((unsigned)time(NULL));    /* set randam seed */
    for (i = 0; i < QUEUE_LENGTH; i++){
    dice:
        r = rand();
        col = r % mod;
        r = rand();
        row = r % mod;

        for (j = 0; j < i; j++){
            if ((indexs[j][0] == col) && (indexs[j][1] == row)){
                goto dice;
            }
        }

        indexs[i][0] = col;
        indexs[i][1] = row;
        r = rand();
        indexs[i][2] = r;

        matrix_set_item(matrix, NODE_VALUE_INTEGER, &r, sizeof(int), indexs[i][0], indexs[i][1]);
    }

    for (i = 0; i < QUEUE_LENGTH; i++){
        matrix_get_item(matrix, NODE_VALUE_INTEGER, &popval, &size, indexs[i][0], indexs[i][1]);
        CU_ASSERT_EQUAL(popval, indexs[i][2]);
    }

    /* dispose the matrix. */
    matrix_dispose(matrix);
    return 0;
}

void test_matrix_multi_threads(void){
    test_multi_threads(ThreadExec);
}

void test_matrix_resize(void){
    char buf[32] = { 0x00 };
    char * string1 = "this is matrix[0, 0].\n";
    char * string2 = "[1, 1], done.\n";
    char * string3 = "hello, [2, 0].\n";
    char * string4 = "[3, 1] get ready.\n";
    char * string5 = "acknowledge, [2, 4].\n";
    char * string6 = "weclome to [4, 4].\n";
    char * string7 = "there may be a bug. [2, 2].\n";
    char * string8 = "a bug may lie here. [2, 3].\n";
    int ret = 0, size = 0;

    LIBXW_MANAGED_MATRIX matrix = NULL;

    matrix = matrix_create(NODE_VALUE_CSTRING, 3, 2);
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 0);

    matrix_set_item(matrix, NODE_VALUE_CSTRING, string1, strlen(string1), 0, 0);    /* +1 */
    matrix_set_item(matrix, NODE_VALUE_CSTRING, string2, strlen(string2), 1, 1);    /* +1 */
    matrix_set_item(matrix, NODE_VALUE_CSTRING, string3, strlen(string3), 2, 0);    /* +1 */
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 3);

    ret = matrix_set_item(matrix, NODE_VALUE_CSTRING, string5, strlen(string5), 2, 4);
    CU_ASSERT_EQUAL(ret, LIBXW_ERRNO_ROWINDEX_OUTRANGE);
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 3);

    matrix_resize(matrix, 5, 5, BOOLEAN_FALSE);
    matrix_set_item(matrix, NODE_VALUE_CSTRING, string5, strlen(string5), 2, 4);    /* +1 */
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 4);
    matrix_get_item(matrix, NODE_VALUE_CSTRING, &buf, &size, 2, 4);
    CU_ASSERT_NSTRING_EQUAL(buf, string5, size);

    matrix_set_item(matrix, NODE_VALUE_CSTRING, string6, strlen(string6), 4, 4);    /* +1 */
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 5);
    matrix_get_item(matrix, NODE_VALUE_CSTRING, &buf, &size, 4, 4);
    CU_ASSERT_NSTRING_EQUAL(buf, string6, size);
    matrix_get_item(matrix, NODE_VALUE_CSTRING, &buf, &size, 0, 0);
    CU_ASSERT_NSTRING_EQUAL(buf, string1, size);

    ret = matrix_resize(matrix, 2, 1, BOOLEAN_FALSE);                               
    CU_ASSERT_EQUAL(ret, LIBXW_ERRNO_INVALIDOPRATION);
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 5);

    matrix_resize(matrix, 3, 2, BOOLEAN_TRUE);                                      /* -2 */
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 3);

    matrix_resize(matrix, 4, 1, BOOLEAN_TRUE);                                      /* -1 */
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 2);
    matrix_set_item(matrix, NODE_VALUE_CSTRING, string8, strlen(string8), 1, 0);    /* +1 */
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 3);
    matrix_get_item(matrix, NODE_VALUE_CSTRING, &buf, &size, 1, 0);
    CU_ASSERT_NSTRING_EQUAL(buf, string8, size);

    matrix_resize(matrix, 2, 1, BOOLEAN_TRUE);                                      /* -1 */
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 2);
    matrix_get_item(matrix, NODE_VALUE_CSTRING, &buf, &size, 0, 0);
    CU_ASSERT_NSTRING_EQUAL(buf, string1, size);
    ret = matrix_set_item(matrix, NODE_VALUE_CSTRING, string3, strlen(string3), 2, 0);
    CU_ASSERT_EQUAL(ret, LIBXW_ERRNO_COLINDEX_OUTRANGE);
    matrix_set_item(matrix, NODE_VALUE_CSTRING, string7, strlen(string7), 1, 0);
    CU_ASSERT_EQUAL(matrix_count_items(matrix), 2);
    matrix_get_item(matrix, NODE_VALUE_CSTRING, &buf, &size, 1, 0);
    CU_ASSERT_NSTRING_EQUAL(buf, string7, size);
}

static CU_TestInfo testcase[] = {
    { "test_matrix_basic:", test_matrix_basic },
    { "test_matrix_multi_threads", test_matrix_multi_threads },
    { "test_matrix_resize", test_matrix_resize },
    CU_TEST_INFO_NULL
};

static CU_SuiteInfo suites[] = {
#ifdef CUNIT_213
    { "Suite of matrix test", suite_success_init, suite_success_clean, NULL, NULL, testcase },
#else
    { "Suite of matrix test", suite_success_init, suite_success_clean, testcase },
#endif
    CU_SUITE_INFO_NULL
};

void AddMatrixTests(){
    if (NULL != CU_get_registry()){
        if (!CU_is_test_running()){
            if (CUE_SUCCESS != CU_register_suites(suites)){
                exit(EXIT_FAILURE);
            }
        }
    }
}
