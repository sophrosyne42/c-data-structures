// stack.h
// Katherine Agent
// Fall 2026
// CS 317
// Project: RE to FA
//
// This file provides a standard implementation of a stack with type-erasure.

#include "stack.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

stack_t* stack_init(size_t data_size) {
    stack_t* stack = malloc(sizeof(stack_t));

    stack->length = 0;
    stack->data_size = data_size;
    stack->top = NULL;

    return stack;
}

// Callers must only push data whose type is the same
// as the type that the stack was initiated with using
// `sizeof(type)`. Otherwise, badness can occur.
void stack_push(stack_t* stack, void* data) {
    stack_node_t* new_node = malloc(sizeof(stack_node_t));
    void* new_data = malloc(stack->data_size);
    if (!new_node || !new_data) {
        fprintf(stderr, "Malloc failed");
    }
    new_node->data = new_data;

    // WARN: This implicitly dereferences `data`. Callers had better ensure that
    // data is a valid pointer. This is a point of fragility of this implementation.
    memcpy(new_node->data, data, stack->data_size);

    stack_node_t* cur_node = stack->top;
    new_node->next = cur_node;
    stack->top = new_node;
    stack->length += 1;

    return;
}
#define stack_push(stack, data) stack_push((stack), (void*)(data))


// Caller needs to cast the result buffer to a type that the 
// caller initiated the stack with.
//
// This function also frees the node that held the top data.
// The caller is responsible for whatever memory is held
// by their provided buffer, but not the memory used by the
// stack.
void* stack_pop(stack_t* stack, void* buffer) {
    if (stack == NULL) return NULL;
    stack_node_t* top = stack->top;
    if (top == NULL || stack->length == 0) {
        return NULL;
    } else {
        stack_node_t* new_top = top->next;

        memcpy(buffer, top->data, stack->data_size);

        stack->top = new_top;
        stack->length -= 1;

        free(top->data);
        free(top);

        return buffer;
    }
}
#define stack_pop(stack, buffer) stack_pop((stack), (void*)(buffer))

void stack_free(stack_t* stack) {
    char throwaway[stack->data_size];
    while (stack->top) {
        stack_pop(stack, &throwaway);
    }
    free(stack);
    return;
}

int stack_length(stack_t* stack) {
    if (stack == NULL) return -1;
    else return stack->length;
}
