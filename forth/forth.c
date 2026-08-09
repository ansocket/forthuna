#include "forth.h"
#include "forth_core.h"

extern const forth_unit_t* FORTH_ADDR;

int forth_init(forth_t* vm, forth_init_t* init_desc)
{
    vm->memory = init_desc->memory;
    vm->sp_start = init_desc->stack + init_desc->sp_cells;
    vm->rsp_start = init_desc->rstack + init_desc->rsp_cells;
    vm->output_cb = NULL;
    vm->input_cb = NULL;
    vm->sp = vm->sp_start;
    vm->rsp = vm->rsp_start;
    vm->here = (char*)vm->memory + init_desc->tib_bytes;
    vm->current = (forth_unit_t*)FORTH_ADDR;
    vm->tib = (char*)vm->memory;
    vm->in = 0;
    vm->status = 0;
    vm->base = 10;
    vm->source = vm->tib;
    vm->local_echo = init_desc->local_echo;
    vm->state = FORTH_STATE_INTERPRET;
    return 0;
}

int forth_process(forth_t* vm)
{
    vm->pc = NULL;
    vm->in = 0;
    vm->status = 0;
    if(vm->input_cb == NULL) return -1;
    char ch;
    int ch_counter = 0;
    do
    {
        forth_key(vm);
        if((vm->status & FORTH_STATUS_INPUT_ERROR) == FORTH_STATUS_INPUT_ERROR)
        {
            break;
        }
        ch = *(vm->sp++);
        if(vm->local_echo && vm->output_cb)
        {
            if(ch == '\0') 
            {
                vm->output_cb('\n');
            }
            else
            {
                vm->output_cb(ch);
            }
        }
        if(ch == '\x7F')
        {
            if(ch_counter > 0)
            {
                ch_counter--;
            }
            if(vm->local_echo && vm->output_cb)
            {
                vm->output_cb('\b');
                vm->output_cb(' ');
                vm->output_cb('\b');
            }
            continue;
        }
        if(ch == '\n')
        {
            break;
        }
        vm->tib[ch_counter++] = ch;
    }while(ch != '\0');
    *(--vm->sp) = (ptrdiff_t)vm->tib;
    *(--vm->sp) = ch_counter;
    forth_evaluate(vm);
    if(vm->status != 0)
    {
        if(vm->status & FORTH_STATUS_NOTFOUND)
        {
            forth_count(vm);
            forth_type(vm);
            forth_send_report(vm," IS NOTFOUND");
        }
        else if(vm->status & FORTH_STATUS_ZERO_DIVISION)
        {
            forth_send_report(vm,"ZERO_DIVISION");
        }
        else if(vm->status & FORTH_STATUS_STACK_UNDERFLOW)
        {
            forth_send_report(vm,"STACK_UNDERFLOW");
        }
        else if(vm->status & FORTH_STATUS_INPUT_ERROR)
        {
            forth_send_report(vm,"INPUT_ERROR");
        }
        else if(vm->status & FORTH_STATUS_EXECUTE_ERROR)
        {
            forth_send_report(vm,"EXECUTE_ERROR");
        }
        else if(vm->status & FORTH_STATUS_EXECUTE_EXIT)
        {
            forth_send_report(vm,"EXECUTE_EXIT");
        }
        else if(vm->status & FORTH_STATUS_WRONG_STATE)
        {
            forth_send_report(vm,"WRONG_STATE");
        }
        else
        {
            forth_send_report(vm,"INVALID_STATUS");
        }
    }
    else
    {
        if(vm->state == FORTH_STATE_INTERPRET)
        {
            forth_send_report(vm," OK");
        }
        else
        {
            forth_send_report(vm," COMPILED");
        }
        
    }
    return 0;
}

int forth_add_function(forth_t* vm, const char* name, forth_function_t func, size_t flags)
{
    forth_align(vm);
    forth_unit_t* var = (forth_unit_t*)vm->here;
    var->flags = flags;
    var->name = name;
    var->next = vm->current;
    var->cfa = (ptrdiff_t*)var->data;
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
    var->cfa = (ptrdiff_t*)var->data;
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
    var->cfa = (ptrdiff_t*)var->data;
    var->next = vm->current;
    vm->current = var;
    var->data[0] = forth_push;
    var->data[1] = (void*)value;
    var->data[2] = forth_exit;
    vm->here += sizeof(forth_unit_t) + sizeof(size_t)*3;
    return 0;
}
int forth_add_variable_ptr(forth_t* vm, const char* name, ptrdiff_t* ptr)
{
    forth_align(vm);
    forth_unit_t* var = (forth_unit_t*)vm->here;
    var->flags = 0;
    var->name = name;
    var->cfa = (ptrdiff_t*)var->data;
    var->next = vm->current;
    vm->current = var;
    var->data[0] = forth_push;
    var->data[1] = (void*)ptr;
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
int forth_send_report(forth_t* vm, const char* string)
{
    unsigned int counter = 0;
    if((vm->output_cb != NULL) && (string != NULL))
    {
        while(string[counter] != '\0')
        {
            vm->output_cb(string[counter++]);
        }
        vm->output_cb('\n');
    }
    return 0;
}
void forth_exit(forth_t* vm)
{
    vm->status |= FORTH_STATUS_EXECUTE_EXIT;
    vm->pc = (ptrdiff_t*)*(vm->rsp++);
}
int forth_add_dictionary(forth_t* vm, const forth_unit_t* start, forth_unit_t* bridge)
{
    bridge->next = vm->current;
    vm->current = (forth_unit_t*)start;
    return 0;
}