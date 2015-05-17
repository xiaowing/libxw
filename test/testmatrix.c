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


    LIBXW_MANAGED_MATRIX matrix = NULL, matrix2 = NULL;
    matrix = matrix_create(NODE_VALUE_INTEGER, 3, 2);
    CU_ASSERT_PTR_NOT_NULL(matrix);

    matrix_set_item(matrix, NODE_VALUE_INTEGER, &foo, sizeof(int), 1, 0);
    CU_ASSERT_EQUAL(matrix_count_item(matrix), 1);

    matrix_set_item(matrix, NODE_VALUE_INTEGER, &bar, sizeof(int), 2, 1);
    CU_ASSERT_EQUAL(matrix_count_item(matrix), 2);

    matrix_get_item(matrix, NODE_VALUE_INTEGER, &tmp, &size, 1, 0);
    CU_ASSERT_EQUAL(tmp, foo);

    CU_ASSERT_EQUAL(matrix_set_item(matrix, NODE_VALUE_INTEGER, &foo, sizeof(int), 3, 0), LIBXW_ERRNO_COLINDEX_OUTRANGE);

    matrix_delete_item(matrix, 2, 1);
    CU_ASSERT_EQUAL(matrix_count_item(matrix), 1);
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


    CU_ASSERT_EQUAL(matrix_count_item(matrix2), 8);
    matrix_get_item(matrix2, NODE_VALUE_CSTRING, &buf, &size, 0, 0);
    CU_ASSERT_NSTRING_EQUAL(buf, string1, size);
    memset(buf, 0x00, sizeof(buf));
    matrix_delete_item(matrix2, 1, 1);
    matrix_get_item(matrix2, NODE_VALUE_CSTRING, &buf, &size, 3, 1);
    CU_ASSERT_NSTRING_EQUAL(buf, string4, size);
    memset(buf, 0x00, sizeof(buf));
    matrix_get_item(matrix2, NODE_VALUE_CSTRING, &buf, &size, 2, 2);
    CU_ASSERT_NSTRING_EQUAL(buf, string9, size);

    CU_ASSERT_EQUAL(matrix_set_item(matrix2, NODE_VALUE_CSTRING, string6, strlen(string6), 5, 5),
        LIBXW_ERRNO_COLINDEX_OUTRANGE);
}

static CU_TestInfo testcase[] = {
    { "test_matrix_basic:", test_matrix_basic },
    CU_TEST_INFO_NULL
};

static CU_SuiteInfo suites[] = {
    { "Suite of matrix test", suite_success_init, suite_success_clean, NULL, NULL, testcase },
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
