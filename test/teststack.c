#include <stdio.h>
#include <stdlib.h>

#include "libxw.h"

int main(void){
    int foo = 12048;
    int tmp = 0, size=0;

    LIBXW_MANAGED_STACK *stack = NULL;
    stack = stack_create(NODE_VALUE_INTEGER);

    stack_push(stack, NODE_VALUE_INTEGER, &foo, sizeof(int));

    printf("Current count of the stack 0x%p is %d\n", stack, stack_items_counter(stack));

    stack_peek(stack, NODE_VALUE_INTEGER, &tmp, &size);

    printf("The value of tmp is %d.\n", tmp);

    stack_pop(stack, NODE_VALUE_INTEGER, &tmp, &size);

    printf("Current count of the stack 0x%p is %d", stack, stack_items_counter(stack));
}