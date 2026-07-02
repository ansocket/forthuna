#include "forth.h"

int forth_init(forth_t* vm, forth_init_t* init_desc)
{
    vm->memory = init_desc->full_memory;
    vm->rsp_start = vm->memory + init_desc->full_memory_cells;
    vm->sp_start = vm->rsp_start - init_desc->rsp_cells;
    vm->output_cb = NULL;
    vm->input_cb = NULL;
    vm->sp = vm->sp_start;
    vm->rsp = vm->rsp_start;
    return 0;
}

int forth_process(forth_t* vm, char ch)
{

}