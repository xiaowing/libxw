/*
============================================================================
Name        : trimstr.c
Author      : Yi Yi
Version     :
Copyright   : Under Apache License 2.0
Description : Trim the space at the head of rear of a string in C, Ansi-style
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libxw.h"

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {
#endif

#ifdef WIN32
    int __cdecl trimstr(char * str, int len){
#else
    int trimstr(char * str, int len){
#endif

        char * tail = NULL;
        char * front = NULL;
        int cnt = len;

        if (str == NULL){
            return -1;
        }

        if ((*str) == 0x00){
            return 0;
        }


        tail = str + (len - 1);

        /* right trim */
        while (*tail == 0x20 || *tail == 0x09
            || *tail == 0x0a || *tail == 0x0d)
        {
            tail--;
            cnt--;
        }
        *(tail + 1) = 0;

        /* left trim */
        front = str;
        while (*front == 0x20 || *front == 0x09
            || *front == 0x0a || *front == 0x0d)
        {
            front++;
            cnt--;
        }

        tail = str;
        if (front != str) {
            do
            {
                *tail++ = *front++;
            } while (*front != 0x00);
            *tail = 0;
        }

        return cnt;
    }

#ifdef __cplusplus    // If used by C++ code, 
}
#endif
