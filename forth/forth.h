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
static const forth_unit_t arg_unitname __attribute__((section(".rodata"))) = \
{                                               \
    .flags = arg_flags,                             \
    .name = arg_name,                                \
    .next = arg_next,                               \
    .cfa = (ptrdiff_t*)arg_unitname.data,                    \
    .data = {                                   \
        arg_func,                      \
        forth_exit                              \
    }                                           \
}                                              \


typedef struct forth forth_t;
typedef struct forth_unit forth_unit_t;
typedef char (*forth_input_t)();
typedef void (*forth_output_t)(char ch);
typedef void (*forth_function_t)(forth_t* vm);

typedef struct forth_unit
{
    size_t flags;
    const char* name;
    const struct forth_unit* next;
    ptrdiff_t* cfa;
    void* data[];
}forth_unit_t;

typedef struct forth_init
{
    ptrdiff_t* full_memory;
    unsigned int full_memory_cells;
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
    forth_unit_t* context;
    char* tib;
    unsigned int in;
    int status;
    char base;
    char* source;
    forth_state_t state;
    ptrdiff_t i;
    ptrdiff_t j;
    char* hld;
}forth_t;

int forth_init(forth_t* vm, forth_init_t* init_desc);
int forth_process(forth_t* vm);  
int forth_add_function(forth_t* vm, const char* name, forth_function_t func, size_t flags);
int forth_add_variable(forth_t* vm, const char* name, ptrdiff_t default_value);
int forth_add_variable_ptr(forth_t* vm, const char* name, ptrdiff_t* ptr);
int forth_add_constant(forth_t* vm, const char* name, ptrdiff_t value);
int forth_reg_output_cb(forth_t* vm, forth_output_t cb);
int forth_reg_input_cb(forth_t* vm, forth_input_t cb);
int forth_send_report(forth_t* vm, const char* string);
#ifdef __cplusplus
}
#endif
#endif