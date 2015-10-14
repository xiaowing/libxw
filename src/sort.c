/*
============================================================================
Name        : sort.c
Author      : Yi Yi
Version     :
Copyright   : Under Apache License 2.0
Description : The common sorting algorithm for general types.
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libxw.h"

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {
#endif

    static int swap_element(void *element_x, void *element_y, size_t size){
        void *temp = malloc(size);
        if (temp == NULL){
            return LIBXW_ERRNO_NULLOBJECT;
        }

        memcpy(temp, element_x, size);
        memcpy(element_x, element_y, size);
        memcpy(element_y, temp, size);

        free(temp);

        return 0;
    }

    static void quick_sort_recursion(void *arr_ptr, int start_idx, int end_idx, size_t size, 
        int(*fcmp)(const void *, const void *)){
        int pivot_idx = 0, store_idx = 0, i;

        if (start_idx >= end_idx){
            return;
        }

        pivot_idx = start_idx;
        store_idx = pivot_idx + 1;
        if (store_idx > end_idx){
            return;
        }

        for (i = store_idx; i <= end_idx; i++){
            if (fcmp((const void *)((char *)arr_ptr + i * size), 
                (const void *)((char *)arr_ptr + pivot_idx * size)) < 0){
                if (i != store_idx){
                    if (swap_element((void *)((char *)arr_ptr + i * size),
                        (void *)((char *)arr_ptr + store_idx * size), size) < 0){
                        exit(EXIT_PROCESS_DEBUG_EVENT);
                    }
                }
                store_idx++;
            }
        }

        store_idx--;

        if (pivot_idx < (store_idx)){
            if (swap_element((void *)((char *)arr_ptr + pivot_idx * size),
                (void *)((char *)arr_ptr + store_idx * size), size) < 0){
                exit(EXIT_PROCESS_DEBUG_EVENT);
            }
        }

        quick_sort_recursion(arr_ptr, start_idx, (store_idx - 1), size, fcmp);
        quick_sort_recursion(arr_ptr, (store_idx + 1), end_idx, size, fcmp);
    }


#ifdef WIN32
    void __cdecl quick_sort(void *base, int nelem, size_t size, int(*fcmp)(const void *, const void *)){
#else
    void quick_sort(void *base, int nelem, size_t size, int(*fcmp)(const void *, const void *)){
#endif
        quick_sort_recursion(base, 0, (nelem - 1), size, fcmp);
    }

#ifdef __cplusplus    // If used by C++ code, 
}
#endif