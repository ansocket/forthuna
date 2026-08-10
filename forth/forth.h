#ifndef FORTH_H
#define FORTH_H

#ifdef __cplusplus
extern "C"{
#endif

#include "stddef.h"

#define FORTH_FLAGS_IMMEDIATE           (1UL << 0)
#define FORTH_FLAGS_COMPILE_ONLY        (1UL << 1)
#define FORTH_FLAGS_INTERPRET_ONLY      (1UL << 2)

#define FORTH_STATUS_NOTFOUND           (1UL << 0)
#define FORTH_STATUS_ZERO_DIVISION      (1UL << 1)
#define FORTH_STATUS_STACK_UNDERFLOW    (1UL << 2)
#define FORTH_STATUS_INPUT_ERROR        (1UL << 3)
#define FORTH_STATUS_EXECUTE_ERROR      (1UL << 4)
#define FORTH_STATUS_EXECUTE_EXIT       (1UL << 5)
#define FORTH_STATUS_WRONG_STATE        (1UL << 6)

#define FORTH_TRUE                      (-1L)
#define FORTH_FALSE                     (0L)

#define FORTH_CONTAINER_OF(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)((char *)__mptr - offsetof(type,member));})

#define FORTH_FUNC_REG(arg_unitname, arg_flags, arg_func, arg_name, arg_next) \
static const forth_unit_t arg_unitname = \
{                                               \
    .flags = arg_flags,                             \
    .name = arg_name,                                \
    .next = (forth_unit_t*)arg_next,                               \
    .cfa = (ptrdiff_t*)arg_unitname.data,                    \
    .data = {                                   \
        arg_func,                      \
        forth_exit                              \
    }                                           \
} 
                                             
#define FORTH_FUNC_RAM_REG(arg_unitname, arg_flags, arg_func, arg_name, arg_next) \
static forth_unit_t arg_unitname = \
{                                               \
    .flags = arg_flags,                             \
    .name = arg_name,                                \
    .next = arg_next,                               \
    .cfa = (ptrdiff_t*)arg_unitname.data,                    \
    .data = {                                   \
        arg_func,                      \
        forth_exit                              \
    }                                           \
}

#if __SIZEOF_PTRDIFF_T__ == 8
typedef __int128_t cell_double;
typedef __uint128_t unsigned_cell_double;
#elif __SIZEOF_PTRDIFF_T__ == 4
typedef long long cell_double;
typedef unsigned long long unsigned_cell_double;
#elif __SIZEOF_PTRDIFF_T__ == 2
typedef long cell_double;
typedef unsigned long unsigned_cell_double;
#endif

typedef struct forth forth_t;
typedef struct forth_unit forth_unit_t;
typedef int (*forth_input_t)(char* data, int maxlen);
typedef void (*forth_output_t)(const char* data, int len);
typedef void (*forth_function_t)(forth_t* vm);

typedef struct forth_unit
{
    size_t flags;
    const char* name;
    struct forth_unit* next;
    ptrdiff_t* cfa;
    void* data[];
}forth_unit_t;

typedef struct forth_init
{
    ptrdiff_t* memory;
    ptrdiff_t* stack;
    ptrdiff_t* rstack;
    unsigned int memory_cells;
    unsigned int sp_cells;
    unsigned int rsp_cells;
    unsigned int tib_bytes;
    char local_echo;
}forth_init_t;

typedef enum
{
    FORTH_STATE_INTERPRET,
    FORTH_STATE_COMPILE
}forth_state_t;

typedef struct forth
{
    ptrdiff_t* memory;
    ptrdiff_t* sp_start;
    ptrdiff_t* rsp_start;
    forth_output_t output_cb;
    char local_echo;
    forth_input_t input_cb;

    ptrdiff_t* sp;
    ptrdiff_t* rsp;
    ptrdiff_t* pc;
    char* here;
    forth_unit_t* current;
    char* tib;
    unsigned int tib_size;
    unsigned int in;
    int status;
    ptrdiff_t base;
    char* source;
    forth_state_t state;
    ptrdiff_t i;
    ptrdiff_t j;
    char* hld;
}forth_t;

void forth_exit(forth_t* vm);
int forth_init(forth_t* vm, forth_init_t* init_desc);
int forth_process(forth_t* vm);  
int forth_add_function(forth_t* vm, const char* name, forth_function_t func, size_t flags);
int forth_add_variable(forth_t* vm, const char* name, ptrdiff_t default_value);
int forth_add_variable_ptr(forth_t* vm, const char* name, ptrdiff_t* ptr);
int forth_add_constant(forth_t* vm, const char* name, ptrdiff_t value);
int forth_reg_output_cb(forth_t* vm, forth_output_t cb);
int forth_reg_input_cb(forth_t* vm, forth_input_t cb);
int forth_send_report(forth_t* vm, const char* string);
int forth_add_dictionary(forth_t* vm, const forth_unit_t* start, forth_unit_t* bridge);

#ifdef __cplusplus
}
#endif
#endif