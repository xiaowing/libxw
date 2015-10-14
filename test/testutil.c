#include <stdio.h>
#include <stdlib.h>

#include "libxw.h"
#include "CUnit.h"

extern int suite_success_init(void);
extern int suite_success_clean(void);

typedef struct sort_element{
    int a;
    int b;
}SORT_ELEMENT;

#define ELEMENT_NUM     5
#define ELEMENT_NUM_MIN 2
#define EPSINON         0.0000001

int compare(const void *p, const void *q)
{
    int ret;
    SORT_ELEMENT x = *(const SORT_ELEMENT *)p;
    SORT_ELEMENT y = *(const SORT_ELEMENT *)q;

    /* Avoid return x - y, which can cause undefined behaviour
    because of signed integer overflow. */
    if ((x.a == y.a) && (x.b == y.b))
        ret = 0;
    else if ((x.a < y.a) && (x.b < y.b))
        ret = -1;
    else
        ret = 1;

    return ret;
}

int compare_float(const void *p, const void *q)
{
    int ret;
    float x = *(const float *)p;
    float y = *(const float *)q;


    if ((x - y) > EPSINON)
        ret = 1;
    else if ((x - y) < -EPSINON)
        ret = -1;
    else
        ret = 0;

    return ret;
}

int compare_char(const void *p, const void *q)
{
    int ret;
    char x = *(const char *)p;
    char y = *(const char *)q;

    if (x > y)
        ret = 1;
    else if (x < y)
        ret = -1;
    else
        ret = 0;

    return ret;
}



void test_trimstr(void){
    char test_string[256] = { "   hello, world.\t" };
    int ret = 0;

    if ((ret = trimstr(test_string, 17)) > 0){
        CU_ASSERT_EQUAL(ret, 13);
        CU_ASSERT_STRING_EQUAL(test_string, "hello, world.");
    }
}


void test_convstr(void){
    char test_string[256] = { "hello, world." };

    if (convstr(test_string) > 0){
        CU_ASSERT_STRING_EQUAL(test_string, ",olleh .dlrow");
    }
}

void test_qsort_struct(void){
    SORT_ELEMENT test_arr[ELEMENT_NUM];
    memset(test_arr, 0x00, ELEMENT_NUM * sizeof(SORT_ELEMENT));

    /* set the value of all elements. */
    test_arr[0].a = 128;
    test_arr[0].b = 256;
    
    test_arr[1].a = 2;
    test_arr[1].b = 789;

    test_arr[2].a = 5;
    test_arr[2].b = 11;

    test_arr[3].a = 64;
    test_arr[3].b = 255;

    test_arr[4].a = 1024;
    test_arr[4].b = 2048;

    quick_sort(test_arr, ELEMENT_NUM, sizeof(SORT_ELEMENT), compare);

    CU_ASSERT_EQUAL(test_arr[0].a, 5);
    CU_ASSERT_EQUAL(test_arr[0].b, 11);

    CU_ASSERT_EQUAL(test_arr[1].a, 64);
    CU_ASSERT_EQUAL(test_arr[1].b, 255);

    CU_ASSERT_EQUAL(test_arr[2].a, 128);
    CU_ASSERT_EQUAL(test_arr[2].b, 256);

    CU_ASSERT_EQUAL(test_arr[3].a, 2);
    CU_ASSERT_EQUAL(test_arr[3].b, 789);

    CU_ASSERT_EQUAL(test_arr[4].a, 1024);
    CU_ASSERT_EQUAL(test_arr[4].b, 2048);
}

void test_qsort_float(void){
    float test_arr[ELEMENT_NUM];
    memset(test_arr, 0x00, ELEMENT_NUM * sizeof(float));

    test_arr[0] = 3.14;
    test_arr[1] = 2.67;
    test_arr[2] = -3.15;
    test_arr[3] = 92.68;
    test_arr[4] = 0.0003;

    quick_sort(test_arr, ELEMENT_NUM, sizeof(float), compare_float);

    CU_ASSERT_DOUBLE_EQUAL(test_arr[0], -3.15, 0.001);
    CU_ASSERT_DOUBLE_EQUAL(test_arr[1], 0.0003, 0.001);
    CU_ASSERT_DOUBLE_EQUAL(test_arr[2], 2.67, 0.001);
    CU_ASSERT_DOUBLE_EQUAL(test_arr[3], 3.14, 0.001);
    CU_ASSERT_DOUBLE_EQUAL(test_arr[4], 92.68, 0.001);
}

void test_qsort_char(void){
    char test_arr[ELEMENT_NUM_MIN];
    memset(test_arr, 0x00, ELEMENT_NUM_MIN * sizeof(char));

    test_arr[0] = 120;
    test_arr[1] = 2;

    quick_sort(test_arr, ELEMENT_NUM_MIN, sizeof(char), compare_char);

    CU_ASSERT_EQUAL(test_arr[0], 2);
    CU_ASSERT_EQUAL(test_arr[1], 120);
}

void test_bsort_float(void){
    float test_arr[ELEMENT_NUM];
    memset(test_arr, 0x00, ELEMENT_NUM * sizeof(float));

    test_arr[0] = 3.14;
    test_arr[1] = 2.67;
    test_arr[2] = -3.15;
    test_arr[3] = 92.68;
    test_arr[4] = 0.0003;

    bubble_sort(test_arr, ELEMENT_NUM, sizeof(float), compare_float);

    CU_ASSERT_DOUBLE_EQUAL(test_arr[0], -3.15, 0.001);
    CU_ASSERT_DOUBLE_EQUAL(test_arr[1], 0.0003, 0.001);
    CU_ASSERT_DOUBLE_EQUAL(test_arr[2], 2.67, 0.001);
    CU_ASSERT_DOUBLE_EQUAL(test_arr[3], 3.14, 0.001);
    CU_ASSERT_DOUBLE_EQUAL(test_arr[4], 92.68, 0.001);
}

static CU_TestInfo testcase[] = {
    { "test_trimstr:", test_trimstr },
    { "test_convstr:", test_convstr },
    { "test_qsort_struct", test_qsort_struct },
    { "test_qsort_float", test_qsort_float },
    { "test_qsort_char", test_qsort_char },
    { "test_bsort_float", test_bsort_float },
    CU_TEST_INFO_NULL
};

static CU_SuiteInfo suites[] = {
    { "Suite of utility test", suite_success_init, suite_success_clean, NULL, NULL, testcase },
    CU_SUITE_INFO_NULL
};

void AddUtilityTests(){
    if (NULL != CU_get_registry()){
        if (!CU_is_test_running()){
            if (CUE_SUCCESS != CU_register_suites(suites)){
                exit(EXIT_FAILURE);
            }
        }
    }
}