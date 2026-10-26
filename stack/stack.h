// stack.h
// Katherine Agent
// Fall 2026
// CS 317
// Project: RE to FA
//
// This file provides the interface for the standard implementation 
// of a generic stack given in stack.c

#ifndef STACK_H
#define STACK_H

#include <sys/types.h>

typedef struct stack_node_t {
    void* data;
    struct stack_node_t* next;
} stack_node_t;

typedef struct stack_t {
    size_t length;
    size_t data_size;
    stack_node_t* top;
} stack_t;

stack_t* stack_init(size_t data_size);

void stack_push(stack_t* stack, void* data);

void* stack_pop(stack_t* stack, void* buffer);

void stack_free(stack_t* stack);

int stack_length(stack_t* stack);

#endif
