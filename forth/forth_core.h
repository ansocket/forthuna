#ifndef FORTH_CORE_H
#define FORTH_CORE_H
#ifdef __cplusplus
extern "C"{
#endif

#include "forth.h"

void forth_exit(forth_t* vm);
void forth_create(forth_t* vm);
void forth_align(forth_t* vm);
void forth_variable_cmd(forth_t* vm);
void forth_push(forth_t * vm);
void forth_tick(forth_t* vm);
void forth_execute(forth_t* vm);
void forth_evaluate(forth_t* vm);
void forth_type(forth_t* vm);
void forth_find(forth_t* vm);
#ifdef __cplusplus
}
#endif
#endif