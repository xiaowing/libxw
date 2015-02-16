/*
============================================================================
Name        : convstr.c
Author      : Yi Yi
Version     :
Copyright   : Under Simplified BSD License
Description : Converse string in C, Ansi-style
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
    int __cdecl convstr(char * str){
#else
    int convstr(char * str){
#endif
        char * rear = NULL;

        if (str == NULL){
            return -1;
        }

        if ((*str) == 0x00){
            return 0;
        }

        do{
            for (rear = str; (*rear != ' ') && (*rear != '\0'); rear++){
                ;;
            }

            for ((--rear); (*str != ' ') && (*str != '\0'); rear--, str++){
                if (str < rear){
                    *rear = (*rear) + (*str);
                    *str = (*rear) - (*str);
                    *rear = (*rear) - (*str);
                }
            }

            if (*str == ' '){
                str++;
            }
        } while (*str != '\0');

        return 1;

    }

#ifdef __cplusplus    // If used by C++ code, 
}
#endif
