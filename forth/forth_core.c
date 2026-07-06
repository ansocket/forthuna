#include "forth.h"
#include "forth_core.h"
#include "string.h"
#include "stdlib.h"

char forth_check_sp_underflow(forth_t* vm, unsigned int num)
{
    if(vm->sp + num > vm->sp_start)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
char forth_check_rsp_underflow(forth_t* vm, unsigned int num)
{
    if(vm->rsp + num > vm->rsp_start)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void forth_push(forth_t * vm)
{
    *(--vm->sp) = *(ptrdiff_t*)(vm->pc++);
}
void forth_variable_cmd(forth_t* vm)
{
    *(--vm->sp) = (ptrdiff_t)(vm->pc + 1);
}
void forth_exit(forth_t* vm)
{
    vm->status |= FORTH_STATUS_EXECUTE_EXIT;
    vm->pc = (ptrdiff_t*)*(vm->rsp++);
} 
void forth_create(forth_t* vm)
{

} 
void forth_align(forth_t* vm)
{
    if((ptrdiff_t)vm->here % sizeof(ptrdiff_t) > 0)
    {
        vm->here += sizeof(ptrdiff_t) - (ptrdiff_t)vm->here % sizeof(ptrdiff_t);
    }
}
void forth_tick(forth_t* vm)
{
    char* save_ptr;
    char* token = strtok_r(&vm->source[vm->in]," ",&save_ptr);
    if(token != NULL)
    {
        vm->in += (save_ptr - &vm->source[vm->in]);
    }
    else
    {
        vm->status |= FORTH_STATUS_NOTFOUND;
        return;
    }
    forth_unit_t* p = vm->current;
    while(p != NULL)
    {
        if(strcmp(p->name,token) == 0)
        {
            *(--vm->sp) = (ptrdiff_t)p->cfa;
            break;
        }
        p = (forth_unit_t*)p->next;
    }
    vm->status |= FORTH_STATUS_NOTFOUND;
}
void forth_execute(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(--vm->rsp) = (ptrdiff_t)vm->pc;
    vm->pc = (ptrdiff_t*)*(vm->sp++);
    while(!(vm->status & FORTH_STATUS_EXECUTE_EXIT))
    {
        forth_function_t func = *(forth_function_t*)(vm->pc++);
        if(func != NULL)
        {
            func(vm);
        }
        else
        {
            vm->status |= FORTH_STATUS_EXECUTE_ERROR;
            break;
        }
    }
    vm->status &= ~FORTH_STATUS_EXECUTE_EXIT;
}
void forth_evaluate(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    int counter = *(vm->sp++); /* TODO: add length check */
    char* str = (char*)*(vm->sp++); 
    char* save_ptr = str;
    vm->in = 0;
    
    char* token; 
    do
    {
        token = strtok_r(&vm->source[vm->in]," ",&save_ptr);
        if(token == NULL)
        {
            break;
        }
        vm->in += (save_ptr - &vm->source[vm->in]);
        forth_unit_t* p = vm->current;
        while(p != NULL)
        {
            if(strcmp(p->name,token) == 0)
            {
                *(--vm->sp) = (ptrdiff_t)p->cfa;
                break;
            }
            p = (forth_unit_t*)p->next;
        }
        if(p != NULL)
        {
            if((vm->state == FORTH_STATE_INTERPRET) || (p->flags & FORTH_FLAGS_IMMEDIATE))
            {
                forth_execute(vm);
            }
            else
            {
                *((ptrdiff_t*)vm->here) = (ptrdiff_t)forth_push;
                vm->here += sizeof(ptrdiff_t);
                *((ptrdiff_t*)vm->here) = *(vm->sp++);
                vm->here += sizeof(ptrdiff_t);
                *((ptrdiff_t*)vm->here) = (ptrdiff_t)forth_execute;
                vm->here += sizeof(ptrdiff_t);
            }
        }
        else
        {
            char* end;
            long int result = strtol(token, &end, vm->base);
            
            if(*end == 0)
            {
                if(vm->state == FORTH_STATE_INTERPRET)
                {
                    *(--vm->sp) = result;
                }
                else
                {
                    *((ptrdiff_t*)vm->here) = (ptrdiff_t)forth_push;
                    vm->here += sizeof(ptrdiff_t);
                    *((ptrdiff_t*)vm->here) = result;
                    vm->here += sizeof(ptrdiff_t);
                }
                
            }
            else
            {
                vm->status |= FORTH_STATUS_NOTFOUND;
                break;
            }
        }

    } while (token != NULL);
}
void forth_plus(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = *(vm->sp) + *(vm->sp + 1);
}
void forth_colon(forth_t* vm)
{
    char* save_ptr;
    char* token = strtok_r(&vm->source[vm->in]," ",&save_ptr);
    if(token != NULL)
    {
        vm->in += (save_ptr - &vm->source[vm->in]);
    }
    else
    {
        vm->status |= FORTH_STATUS_NOTFOUND;
        return;
    }
    unsigned int token_len = strlen(token);
    forth_align(vm);
    forth_unit_t* unit = (forth_unit_t*)vm->here;
    vm->here += sizeof(forth_unit_t);
    unit->flags = 0;
    unit->name = (char*)unit->data;
    unit->next = vm->current;
    vm->current = unit;
    memcpy((char*)unit->name,token,token_len);
    vm->here += token_len;
    forth_align(vm);
    unit->cfa = (ptrdiff_t*)vm->here;
    vm->state = FORTH_STATE_COMPILE;
}
void forth_semicolon(forth_t* vm)
{
    if(vm->state != FORTH_STATE_COMPILE)
    {
        vm->status |= FORTH_STATUS_WRONG_STATE;
        return;
    }
    *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_exit;
    vm->here += sizeof(ptrdiff_t);
    vm->state = FORTH_STATE_INTERPRET;
}
void forth_store(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t* addr = (ptrdiff_t*)*(vm->sp++);
    ptrdiff_t value = *(vm->sp++);
    *addr = value;
}
void forth_fetch(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t* addr = (ptrdiff_t*)*(vm->sp++);
    *(--vm->sp) = *addr;
}
void forth_true(forth_t* vm)
{
    *(--vm->sp) = FORTH_TRUE;
}
void forth_false(forth_t* vm)
{
    *(--vm->sp) = FORTH_FALSE;
}
void forth_aligned(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t value = *(vm->sp++);
    if(value % sizeof(ptrdiff_t) > 0)
    {
        value += sizeof(ptrdiff_t) - (ptrdiff_t)value % sizeof(ptrdiff_t);
    }
    *(--vm->sp) = value;
}
void forth_here(forth_t* vm)
{
    *(--vm->sp) = (ptrdiff_t)&vm->here;
}
void forth_current(forth_t* vm)
{
    *(--vm->sp) = (ptrdiff_t)&vm->current;
}
void forth_base(forth_t* vm)
{
    *(--vm->sp) = (ptrdiff_t)vm->base;
}
void forth_immediate(forth_t* vm)
{
    forth_unit_t* var = vm->current;
    var->flags |= FORTH_FLAGS_IMMEDIATE;
}
FORTH_FUNC_REG(forth_exit_unit,0,forth_exit,"EXIT",NULL);
FORTH_FUNC_REG(forth_create_unit,0,forth_create,"CREATE",&forth_exit_unit);
FORTH_FUNC_REG(forth_align_unit,0,forth_align,"ALIGN",&forth_create_unit);
FORTH_FUNC_REG(forth_aligned_unit,0,forth_aligned,"ALIGNED",&forth_align_unit);
FORTH_FUNC_REG(forth_tick_unit,0,forth_tick,"'",&forth_aligned_unit);
FORTH_FUNC_REG(forth_execute_unit,0,forth_execute,"EXECUTE",&forth_tick_unit);
FORTH_FUNC_REG(forth_plus_unit,0,forth_plus,"+",&forth_execute_unit);
FORTH_FUNC_REG(forth_colon_unit,0,forth_colon,":",&forth_plus_unit);
FORTH_FUNC_REG(forth_semicolon_unit,FORTH_FLAGS_IMMEDIATE,forth_semicolon,";",&forth_colon_unit);
FORTH_FUNC_REG(forth_store_unit,0,forth_store,"!",&forth_semicolon_unit);
FORTH_FUNC_REG(forth_fetch_unit,0,forth_fetch,"@",&forth_store_unit);
FORTH_FUNC_REG(forth_true_unit,0,forth_true,"TRUE",&forth_fetch_unit);
FORTH_FUNC_REG(forth_false_unit,0,forth_false,"FALSE",&forth_true_unit);
FORTH_FUNC_REG(forth_here_unit,0,forth_here,"HERE",&forth_false_unit);
FORTH_FUNC_REG(forth_current_unit,0,forth_current,"CURRENT",&forth_here_unit);
FORTH_FUNC_REG(forth_base_unit,0,forth_base,"BASE",&forth_current_unit);
FORTH_FUNC_REG(forth_immediate_unit,0,forth_immediate,"IMMEDIATE",&forth_base_unit);
FORTH_FUNC_REG(forth_unit,0,forth_exit,"FORTH",&forth_immediate_unit);
const forth_unit_t* FORTH_ADDR = &forth_unit;