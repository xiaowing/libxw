#include <stdio.h>
#include <stdlib.h>

#include "libxw.h"
#include "CUnit.h"

extern int suite_success_init(void);
extern int suite_success_clean(void);

void test_trimstr(void){
    char test_string[256] = { "   hello, world.\t" };

    if (trimstr(test_string) > 0){
        CU_ASSERT_STRING_EQUAL(test_string, "hello, world.");
    }
}


void test_convstr(void){
    char test_string[256] = { "hello, world." };

    if (convstr(test_string) > 0){
        CU_ASSERT_STRING_EQUAL(test_string, ",olleh .dlrow");
    }
}

static CU_TestInfo testcase[] = {
    { "test_trimstr:", test_trimstr },
    { "test_convstr:", test_convstr },
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