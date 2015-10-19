#include <stdio.h>
#include <stdlib.h>

#include "Basic.h"
#include "CUnit.h"

#ifdef __cplusplus
extern "C" {
#endif
    void AddStackTests();
    void AddQueueTests();
    void AddMatrixTests();
    void AddUtilityTests();
#ifdef __cplusplus
}
#endif

int main(void){
    if (CU_initialize_registry()){
        fprintf(stderr, " Initialization of Test Registry failed. ");
        exit(EXIT_FAILURE);
    }
    else{
        AddStackTests();
        AddQueueTests();
        AddMatrixTests();
        AddUtilityTests();

        CU_basic_set_mode(CU_BRM_VERBOSE);
        return CU_basic_run_tests();
    }
}
