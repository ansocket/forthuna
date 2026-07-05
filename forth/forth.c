#include "forth.h"
#include "forth_core.h"

extern const forth_unit_t* FORTH_ADDR;

int forth_init(forth_t* vm, forth_init_t* init_desc)
{
    vm->memory = init_desc->full_memory;
    vm->rsp_start = vm->memory + init_desc->full_memory_cells;
    vm->sp_start = vm->rsp_start - init_desc->rsp_cells;
    vm->output_cb = NULL;
    vm->input_cb = NULL;
    vm->sp = vm->sp_start;
    vm->rsp = vm->rsp_start;
    vm->here = (char*)vm->memory + init_desc->tib_bytes;
    vm->current = (forth_unit_t*)FORTH_ADDR;
    vm->context = (forth_unit_t*)FORTH_ADDR;
    vm->tib = (char*)vm->memory;
    vm->in = 0;
    vm->status = 0;
    vm->base = 10;
    vm->source = vm->tib;
    vm->state = FORTH_STATE_INTERPRET;
    return 0;
}

int forth_process(forth_t* vm)
{
    vm->pc = NULL;
    vm->in = 0;
    if(vm->input_cb == NULL) return -1;
    char ch;
    int ch_counter = 0;
    do
    {
        ch = vm->input_cb();
        vm->tib[ch_counter++] = ch;
    }while(ch != '\0');
    *(--vm->sp) = (ptrdiff_t)vm->tib;
    *(--vm->sp) = ch_counter;
    forth_evaluate(vm);
}

int forth_add_function(forth_t* vm, const char* name, forth_function_t func, size_t flags)
{
    forth_align(vm);
    forth_unit_t* var = (forth_unit_t*)vm->here;
    var->flags = flags;
    var->name = name;
    var->next = vm->current;
    var->cfa = var->data;
    vm->current = var;
    var->data[0] = func;
    var->data[1] = forth_exit;
    vm->here += sizeof(forth_unit_t) + sizeof(size_t)*2;
    return 0;
}
int forth_add_variable(forth_t* vm, const char* name, ptrdiff_t default_value)
{
    forth_align(vm);
    forth_unit_t* var = (forth_unit_t*)vm->here;
    var->flags = 0;
    var->name = name;
    var->cfa = var->data;
    var->next = vm->current;
    vm->current = var;
    var->data[0] = forth_variable_cmd;
    var->data[1] = forth_exit;
    var->data[2] = (void*)default_value;
    vm->here += sizeof(forth_unit_t) + sizeof(size_t)*3;
    return 0;
}
int forth_add_constant(forth_t* vm, const char* name, ptrdiff_t value)
{
    forth_align(vm);
    forth_unit_t* var = (forth_unit_t*)vm->here;
    var->flags = 0;
    var->name = name;
    var->cfa = var->data;
    var->next = vm->current;
    vm->current = var;
    var->data[0] = forth_push;
    var->data[1] = (void*)value;
    var->data[2] = forth_exit;
    vm->here += sizeof(forth_unit_t) + sizeof(size_t)*3;
    return 0;
}
int forth_reg_output_cb(forth_t* vm, forth_output_t cb)
{
    vm->output_cb = cb;
    return 0;
}
int forth_reg_input_cb(forth_t* vm, forth_input_t cb)
{
    vm->input_cb = cb;
    return 0;
}