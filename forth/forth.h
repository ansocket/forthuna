#ifndef FORTH_H
#define FORTH_H

#ifdef __cplusplus
extern "C"{
#endif

#include "stddef.h"
typedef struct forth forth_t;
typedef struct forth_unit forth_unit_t;
typedef void (*forth_input_t)(char ch);
typedef void (*forth_output_t)(const char* data, unsigned int len);
typedef void (*forth_function_t)(forth_t* vm);

typedef struct forth_unit
{
    size_t flags;
    const char* name;
    struct forth_unit* next;
    void* data[];
}forth_unit_t;

typedef struct forth_init
{
    unsigned int sp_cells;
    unsigned int rsp_cells;
    ptrdiff_t* full_memory;
    unsigned int full_memory_cells;
}forth_init_t;

typedef struct forth
{
    ptrdiff_t* sp;
    ptrdiff_t* rsp;
    ptrdiff_t* memory;
    ptrdiff_t* sp_start;
    ptrdiff_t* rsp_start;
    forth_output_t output_cb;
    forth_input_t input_cb;
}forth_t;

int forth_init(forth_t* vm, forth_init_t* init_desc);
int forth_process(forth_t* vm, char ch);  
int forth_add_function(forth_t* vm, const char* name, forth_function_t func, size_t flags);
int forth_add_variable(forth_t* vm, const char* name, ptrdiff_t default_value);
int forth_add_constant(forth_t* vm, const char* name, ptrdiff_t value);
int forth_reg_output_cb(forth_t* vm, forth_output_t cb);
#ifdef __cplusplus
}
#endif
#endif