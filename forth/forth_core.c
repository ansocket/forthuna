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
void forth_rpush(forth_t * vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(--vm->rsp) = *(vm->sp++);
}
void forth_push(forth_t * vm)
{
    *(--vm->sp) = *(ptrdiff_t*)(vm->pc++);
}
void forth_drop(forth_t * vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    vm->sp++;
}
void forth_swap(forth_t * vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp) ^= *(vm->sp + 1);
    *(vm->sp + 1) ^= *(vm->sp);
    *(vm->sp) ^= *(vm->sp + 1);
}
void forth_jmp(forth_t * vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    vm->pc = (ptrdiff_t*)*(vm->sp++);
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
void forth_exit_compile(forth_t* vm)
{
    *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_exit;
    vm->here += sizeof(ptrdiff_t);
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
            return;
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
        *(--vm->sp) = (ptrdiff_t)token;
        
        forth_find(vm);
        char find_result = *(vm->sp++);
        if(find_result != 0)
        {
            if((vm->state == FORTH_STATE_INTERPRET) || (find_result == 1))
            {
                forth_execute(vm);
            }
            else
            {
                *((ptrdiff_t*)vm->here) = (ptrdiff_t)forth_push;
                vm->here += sizeof(ptrdiff_t);
                *((ptrdiff_t*)vm->here) = (ptrdiff_t)*(vm->sp++);
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
                forth_drop(vm);
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
    *(++vm->sp) = *(vm->sp + 1) + *(vm->sp);
}
void forth_minus(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = *(vm->sp + 1) - *(vm->sp);
}
void forth_mul(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = *(vm->sp + 1) * *(vm->sp);
}
void forth_xor(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = *(vm->sp + 1) ^ *(vm->sp);
}
void forth_over(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(--vm->sp) = *(vm->sp + 1);
}
void forth_lshift(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = *(vm->sp + 1) << *(vm->sp);
}
void forth_rshift(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = *(vm->sp + 1) >> *(vm->sp);
}
void forth_mod(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    if(*(vm->sp) == 0)
    {
        vm->status |= FORTH_STATUS_ZERO_DIVISION;
        return;
    }
    *(++vm->sp) = *(vm->sp + 1) % *(vm->sp);
}
void forth_div(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    if(*(vm->sp) == 0)
    {
        vm->status |= FORTH_STATUS_ZERO_DIVISION;
        return;
    }
    *(++vm->sp) = *(vm->sp + 1) / *(vm->sp);
}
void forth_min(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t val1 = *(vm->sp);
    ptrdiff_t val2 = *(vm->sp);
    *(++vm->sp) = (val1 > val2) ? val1 : val2; 
}
void forth_max(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t val1 = *(vm->sp);
    ptrdiff_t val2 = *(vm->sp);
    *(++vm->sp) = (val1 < val2) ? val1 : val2; 
}
void forth_invert(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp) = ~(*(vm->sp));
}
void forth_negate(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    forth_invert(vm);
    *(vm->sp) += 1;
}
void forth_abs(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    if(*(vm->sp) < 0)
    {
        forth_negate(vm);
    }
}
void forth_dup(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp - 1) = *(vm->sp);
    vm->sp--;
}
void forth_question_dup(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    if(*(vm->sp) != 0)
    {
        *(vm->sp - 1) = *(vm->sp);
        vm->sp--;
    }
}
void forth_less(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = (*(vm->sp + 1) < *(vm->sp)) ? FORTH_TRUE : FORTH_FALSE;
}
void forth_greater(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = (*(vm->sp + 1) > *(vm->sp)) ? FORTH_TRUE : FORTH_FALSE;
}
void forth_equal(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = (*(vm->sp + 1) == *(vm->sp)) ? FORTH_TRUE : FORTH_FALSE;
}
void forth_and(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = (*(vm->sp + 1) && *(vm->sp)) ? FORTH_TRUE : FORTH_FALSE;
}
void forth_or(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(++vm->sp) = (*(vm->sp + 1) || *(vm->sp)) ? FORTH_TRUE : FORTH_FALSE;
}
void forth_zero_less(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp) = (*(vm->sp) < 0) ? FORTH_TRUE : FORTH_FALSE;
}
void forth_zero_equal(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp) = (*(vm->sp) == 0) ? FORTH_TRUE : FORTH_FALSE;
}
void forth_one_plus(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp) += 1;
}
void forth_one_minus(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp) -= 1;
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
    *((char*)unit->name + token_len) = '\0';
    vm->here += token_len + 1;
    forth_align(vm);
    unit->cfa = (ptrdiff_t*)vm->here;
    vm->state = FORTH_STATE_COMPILE;
}
void forth_semicolon(forth_t* vm)
{
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
void forth_plus_store(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t* addr = (ptrdiff_t*)*(vm->sp++);
    ptrdiff_t value = *(vm->sp++);
    *addr += value;
}
void forth_с_fetch(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    char* addr = (char*)*(vm->sp++);
    *(--vm->sp) = *addr;
}
void forth_с_store(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    char* addr = (char*)*(vm->sp++);
    char ch = (char)*(vm->sp++);
    *addr = ch;
}
void forth_сhar(forth_t* vm)
{
    char* save_ptr;
    char* token = strtok_r(&vm->source[vm->in]," ",&save_ptr);
    if(token != NULL)
    {
        vm->in += (save_ptr - &vm->source[vm->in]);
    }
    else
    {
        vm->status |= FORTH_STATUS_INPUT_ERROR;
        return;
    }
    *(--vm->sp) = (ptrdiff_t)token[0];
}
void forth_bracket_сhar(forth_t* vm)
{
    char* save_ptr;
    char* token = strtok_r(&vm->source[vm->in]," ",&save_ptr);
    if(token != NULL)
    {
        vm->in += (save_ptr - &vm->source[vm->in]);
    }
    else
    {
        vm->status |= FORTH_STATUS_INPUT_ERROR;
        return;
    }
    *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_push;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = (ptrdiff_t)token[0];
    vm->here += sizeof(ptrdiff_t);
}
void forth_с_comma(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    char ch = (char)*(vm->sp++);
    *(vm->here++) = ch;
}
void forth_сhar_plus(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    *(vm->sp) += 1;
}
void forth_сhars(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    /* Nothing to do here :) */
}
void forth_squote(forth_t* vm)
{
    char* save_ptr;
    char* token = strtok_r(&vm->source[vm->in],"\"",&save_ptr);
    if(token != NULL)
    {
        vm->in += (save_ptr - &vm->source[vm->in]);
    }
    else
    {
        vm->status |= FORTH_STATUS_INPUT_ERROR;
        return;
    }

    unsigned int token_len = strlen(token);
    ptrdiff_t** jmp_addr;
    if(vm->state == FORTH_STATE_COMPILE)
    {
        *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_push;
        vm->here += sizeof(ptrdiff_t);
        jmp_addr = (ptrdiff_t**)vm->here;
        vm->here += sizeof(ptrdiff_t);
        *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_jmp;
        vm->here += sizeof(ptrdiff_t);
    }

    char* str_addr = vm->here;
    memcpy(str_addr,token,token_len);
    if(vm->state == FORTH_STATE_COMPILE)
    {
        vm->here += token_len;
        forth_align(vm);
        *jmp_addr = (ptrdiff_t*)vm->here;
        *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_push;
        vm->here += sizeof(ptrdiff_t);
        *(ptrdiff_t*)vm->here = (ptrdiff_t)str_addr;
        vm->here += sizeof(ptrdiff_t);
        *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_push;
        vm->here += sizeof(ptrdiff_t);
        *(ptrdiff_t*)vm->here = token_len;
        vm->here += sizeof(ptrdiff_t);
    }
    else
    {
        *(--vm->sp) = (ptrdiff_t)str_addr;
        *(--vm->sp) = token_len;
    }
}
void forth_dot_quote(forth_t* vm)
{
    char* save_ptr;
    char* token = strtok_r(&vm->source[vm->in],"\"",&save_ptr);
    if(token != NULL)
    {
        vm->in += (save_ptr - &vm->source[vm->in]);
    }
    else
    {
        vm->status |= FORTH_STATUS_INPUT_ERROR;
        return;
    }

    unsigned int token_len = strlen(token);
    ptrdiff_t** jmp_addr;
    if(vm->state == FORTH_STATE_COMPILE)
    {
        *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_push;
        vm->here += sizeof(ptrdiff_t);
        jmp_addr = (ptrdiff_t**)vm->here;
        vm->here += sizeof(ptrdiff_t);
        *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_jmp;
        vm->here += sizeof(ptrdiff_t);
    }

    char* str_addr = vm->here;
    memcpy(str_addr,token,token_len);
    if(vm->state == FORTH_STATE_COMPILE)
    {
        vm->here += token_len;
        forth_align(vm);
        *jmp_addr = (ptrdiff_t*)vm->here;
        *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_push;
        vm->here += sizeof(ptrdiff_t);
        *(ptrdiff_t*)vm->here = (ptrdiff_t)str_addr;
        vm->here += sizeof(ptrdiff_t);
        *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_push;
        vm->here += sizeof(ptrdiff_t);
        *(ptrdiff_t*)vm->here = token_len;
        vm->here += sizeof(ptrdiff_t);
        *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_type;
        vm->here += sizeof(ptrdiff_t);
    }
    else
    {
        *(--vm->sp) = (ptrdiff_t)str_addr;
        *(--vm->sp) = token_len;
        forth_type(vm);
    }
}
void forth_emit(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    if(vm->output_cb != NULL)
    {
        vm->output_cb((char)*(vm->sp++));
    }
}
void forth_cr(forth_t* vm)
{
    *(--vm->sp) = '\n';
    forth_emit(vm);
}
void forth_bl(forth_t* vm)
{
    *(--vm->sp) = ' ';
}
void forth_space(forth_t* vm)
{
    forth_bl(vm);
    forth_emit(vm);
}
void forth_type(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    const unsigned int len = *(vm->sp++);
    const char * str = (char*)*(vm->sp++);
    for(unsigned int i = 0; i < len; i++)
    {
        if(vm->output_cb != NULL)
        {
            vm->output_cb(str[i]);
        }
    }

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
void forth_pad(forth_t* vm)
{
    *(--vm->sp) = (ptrdiff_t)vm->here + 100; /* TODO: Maybe make this value configured? */
}
void forth_hld(forth_t* vm)
{
    *(--vm->sp) = (ptrdiff_t)&vm->hld;
}
void forth_less_number_sign(forth_t* vm)
{
    forth_pad(vm);
    vm->hld = (char*)*(vm->sp++);
}
void forth_number_sign(forth_t* vm)
{
    const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t number1 = *(vm->sp++);
    ptrdiff_t number2 = *(vm->sp++);
    ptrdiff_t res;
    if(number2 != 0)
    {
        res = number2 % vm->base;
        *(--vm->sp) = number2 / vm->base;
        *(--vm->sp) = number1;
    }
    else
    {
        *(--vm->sp) = number2;
        res = number1 % vm->base;
        *(--vm->sp) = number1 / vm->base;
    }
    *(--vm->hld) = digits[abs(res)];
}
void forth_number_sign_s(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t number1 = *(vm->sp);
    ptrdiff_t number2 = *(vm->sp + 1);
    while(1)
    {
        forth_number_sign(vm);
        number1 = *(vm->sp);
        number2 = *(vm->sp + 1);
        if((number1 == 0) && (number2 == 0))
        {
            break;
        }
    }
}
void forth_number_sign_greater(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    vm->sp += 2;
    forth_pad(vm);
    char* pad_addr = (char*)*(vm->sp++);
    *(--vm->sp) = (ptrdiff_t)vm->hld;
    *(--vm->sp) = (ptrdiff_t)(pad_addr - vm->hld);
}
void forth_sign(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    if(*(vm->sp++) < 0)
    {
        *(--vm->hld) = '-';
    }
}
void forth_dot(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    forth_less_number_sign(vm);
    char sign = *(vm->sp) < 0;
    *(--vm->sp) = 0;
    forth_number_sign_s(vm);
    if(sign)
    {
        *(--vm->sp) = -1;
        forth_sign(vm);
    }
    forth_number_sign_greater(vm);
    forth_type(vm);
    forth_space(vm);
}
void forth_current(forth_t* vm)
{
    *(--vm->sp) = (ptrdiff_t)&vm->current;
}
void forth_base(forth_t* vm)
{
    *(--vm->sp) = (ptrdiff_t)&vm->base;
}
void forth_immediate(forth_t* vm)
{
    forth_unit_t* unit = vm->current;
    unit->flags |= FORTH_FLAGS_IMMEDIATE;
}
void forth_find(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    char* token = (char*)*(vm->sp++);
    forth_unit_t* p = vm->current;
    while(p != NULL)
    {
        if(strcmp(p->name,token) == 0)
        {
            if(!((vm->state == FORTH_STATE_INTERPRET) && ((p->flags & FORTH_FLAGS_COMPILE_ONLY) == FORTH_FLAGS_COMPILE_ONLY)))
            {

                break;
            }
            
        }
        p = (forth_unit_t*)p->next;
    }
    if(p != NULL)
    {        
        *(--vm->sp) = (ptrdiff_t)p->cfa;
        if((p->flags & FORTH_FLAGS_IMMEDIATE) == FORTH_FLAGS_IMMEDIATE)
        {
            *(--vm->sp) = 1;
        }
        else
        {
            *(--vm->sp) = -1;
        }
        return;
    }
        *(--vm->sp) = (ptrdiff_t)token;
        *(--vm->sp) = 0;
}
void forth_create(forth_t* vm)
{
    char* save_ptr;
    char* token = strtok_r(&vm->source[vm->in]," ",&save_ptr);
    if(token != NULL)
    {
        vm->in += (save_ptr - &vm->source[vm->in]);
    }
    else
    {
        vm->status |= FORTH_STATUS_INPUT_ERROR;
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
    *((char*)unit->name + token_len) = '\0';
    vm->here += token_len + 1;
    forth_align(vm);
    unit->cfa = (ptrdiff_t*)vm->here;
    
    *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_push;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = (ptrdiff_t)vm->here + sizeof(ptrdiff_t)*4;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_exit;
    vm->here += sizeof(ptrdiff_t)*3;
}
static void forth_does_runtime(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    forth_unit_t* unit = vm->current;
    ptrdiff_t* data_ptr = (ptrdiff_t*)unit->name;
    data_ptr = (ptrdiff_t*)((char*)data_ptr + strlen(unit->name) + 1);
    *(--vm->sp) = (ptrdiff_t)data_ptr;
    forth_aligned(vm);
    data_ptr = (ptrdiff_t*)*(vm->sp++);
    data_ptr[2] = (ptrdiff_t)forth_push;
    data_ptr[3] = *(vm->sp++);
    data_ptr[4] = (ptrdiff_t)forth_jmp;
}
void forth_does(forth_t* vm)
{
    *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_push;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = (ptrdiff_t)vm->here + sizeof(ptrdiff_t)*3;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_does_runtime;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = (ptrdiff_t)forth_exit;
    vm->here += sizeof(ptrdiff_t);
    vm->state = FORTH_STATE_COMPILE;
}
void forth_comma(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    forth_align(vm);
    *(ptrdiff_t*)vm->here = *(vm->sp++);
    vm->here += sizeof(ptrdiff_t);
}
void forth_allot(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    vm->here += *(vm->sp++);
}
void forth_move(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,3))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t num = *(vm->sp++);
    ptrdiff_t* addr2 = (ptrdiff_t*)*(vm->sp++);
    ptrdiff_t* addr1 = (ptrdiff_t*)*(vm->sp++);
    if(num > 0)
    {
        for(unsigned int i = 0; i < num; i++)
        {
            addr2[i] = addr1[i];
        }
    }
}
void forth_cells(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t value = *(vm->sp++);
    *(--vm->sp) = value*sizeof(ptrdiff_t);
}
void forth_cell_plus(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    } 
    *(vm->sp) += sizeof(ptrdiff_t);
}
static void forth_if_runtime(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    } 
    ptrdiff_t expresson_res = *(vm->sp++);
    if(expresson_res != FORTH_FALSE)
    {
        vm->pc++;
    }
    else
    {
        vm->pc = (ptrdiff_t*)*vm->pc;
    }
}
void forth_if(forth_t* vm)
{
    *(forth_function_t*)vm->here = forth_if_runtime;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = 0;
    *(--vm->sp) = (ptrdiff_t)vm->here;
    vm->here += sizeof(ptrdiff_t);
}
void forth_then(forth_t* vm)
{
    ptrdiff_t** addr = (ptrdiff_t**)*(vm->sp++);
    *addr = (ptrdiff_t*)vm->here;
}
static void forth_else_runtime(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    } 
    vm->pc = (ptrdiff_t*)*(vm->pc);
}
void forth_else(forth_t* vm)
{
    ptrdiff_t* addr = (ptrdiff_t*)*(vm->sp++);
    *addr = (ptrdiff_t)vm->here + sizeof(ptrdiff_t)*2;

    *(forth_function_t*)vm->here = forth_else_runtime;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = 0;

    *(--vm->sp) = (ptrdiff_t)vm->here;
    vm->here += sizeof(ptrdiff_t);
}

static void forth_do_start_runtime(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    } 
    if((vm->rsp + 1) < vm->rsp_start)
    {
        vm->j = *(vm->rsp + 1);
    }
    *(--vm->rsp) = *(vm->sp + 1);
    *(--vm->rsp) = *(vm->sp);
    
    vm->i = *(vm->sp);
    vm->sp += 2;
    vm->pc += 3;
}
static void forth_do_continue_runtime(forth_t* vm)
{
    *(--vm->rsp) = (ptrdiff_t)(vm->pc - 1);
}
void forth_do(forth_t* vm)
{
    *(forth_function_t*)vm->here = forth_do_start_runtime;
    vm->here += sizeof(ptrdiff_t);
    *(forth_function_t*)vm->here = forth_push;
    vm->here += sizeof(ptrdiff_t);
    *(--vm->sp) = (ptrdiff_t)vm->here;
    vm->here += sizeof(ptrdiff_t);
    *(forth_function_t*)vm->here = forth_jmp;
    vm->here += sizeof(ptrdiff_t);
    *(forth_function_t*)vm->here = forth_do_continue_runtime;
    vm->here += sizeof(ptrdiff_t);
}
static void forth_loop_runtime(forth_t* vm)
{
    *(vm->rsp + 1) += 1;
    if(*(vm->rsp + 1) < *(vm->rsp + 2))
    {
        vm->i = *(vm->rsp + 1);
        vm->pc = (ptrdiff_t*)*(vm->rsp++);
    }
    else
    {
        vm->pc = (ptrdiff_t*)*(vm->rsp++) - 3;
        vm->rsp += 2;
        vm->i = vm->j;
        if((vm->rsp + 1) < vm->rsp_start)
        {
            vm->j = *(vm->rsp + 1);
        }
    }
}
void forth_loop(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    } 
    *(forth_function_t*)vm->here = forth_loop_runtime;
    vm->here += sizeof(ptrdiff_t);
    ptrdiff_t* addr = (ptrdiff_t*)*(vm->sp++);
    *addr = (ptrdiff_t)vm->here;
}
static void forth_plus_loop_runtime(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    } 
    ptrdiff_t step = *(vm->sp++);
    
    if(step < 0)
    {
        if(*(vm->rsp + 1) > *(vm->rsp + 2))
        {
            *(vm->rsp + 1) += step;
            vm->i = *(vm->rsp + 1);
            vm->pc = (ptrdiff_t*)*(vm->rsp++);
            return;
        }
    }
    else
    {
        if(*(vm->rsp + 1) < *(vm->rsp + 2))
        {
            *(vm->rsp + 1) += step;
            vm->i = *(vm->rsp + 1);
            vm->pc = (ptrdiff_t*)*(vm->rsp++);
            return;
        }
    }
    vm->pc = (ptrdiff_t*)*(vm->rsp++) - 3;
    vm->rsp += 2;
    vm->i = vm->j;
    if((vm->rsp + 1) < vm->rsp_start)
    {
        vm->j = *(vm->rsp + 1);
    }
}
void forth_plus_loop(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    } 
    *(forth_function_t*)vm->here = forth_plus_loop_runtime;
    vm->here += sizeof(ptrdiff_t);
    ptrdiff_t* addr = (ptrdiff_t*)*(vm->sp++);
    *addr = (ptrdiff_t)vm->here;
}
static void forth_leave_runtime(forth_t* vm)
{
    vm->pc = (ptrdiff_t*)*(vm->rsp++) - 3;
    vm->rsp += 2;
    vm->i = vm->j;
    if((vm->rsp + 1) < vm->rsp_start)
    {
        vm->j = *(vm->rsp + 1);
    }
}
void forth_leave(forth_t* vm)
{
    *(forth_function_t*)vm->here = forth_leave_runtime;
    vm->here += sizeof(ptrdiff_t);
}
void forth_unloop(forth_t* vm)
{
    ptrdiff_t ret_addr = *(vm->rsp++);
    vm->rsp += 3;
    vm->i = vm->j;
    if((vm->rsp + 1) < vm->rsp_start)
    {
        vm->j = *(vm->rsp + 1);
    }
    *(--vm->rsp) = ret_addr;
}
void forth_i(forth_t* vm)
{
    *(--vm->sp) = vm->i;
}
void forth_j(forth_t* vm)
{
    *(--vm->sp) = vm->j;
}
void forth_begin(forth_t* vm)
{
    *(--vm->sp) = (ptrdiff_t)vm->here; 
}
static void forth_until_runtime(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t* ret_addr = (ptrdiff_t*)*(vm->sp++);
    ptrdiff_t condition = *(vm->sp++);
    if(condition == FORTH_FALSE)
    {
        vm->pc = ret_addr;
    }
}
void forth_until(forth_t* vm)
{
    ptrdiff_t* ret_addr = (ptrdiff_t*)*(vm->sp++);
    *(forth_function_t*)vm->here = forth_push;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = (ptrdiff_t)ret_addr;
    vm->here += sizeof(ptrdiff_t);
    *(forth_function_t*)vm->here = forth_until_runtime;
    vm->here += sizeof(ptrdiff_t);
}
static void forth_while_runtime(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,2))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t* jmp_addr = (ptrdiff_t*)*(vm->sp++);
    ptrdiff_t condition = *(vm->sp++);
    if(condition == FORTH_FALSE)
    {
        vm->pc = jmp_addr;
    }
}
void forth_while(forth_t* vm)
{
    *(forth_function_t*)vm->here = forth_push;
    vm->here += sizeof(ptrdiff_t);
    *(--vm->sp) = (ptrdiff_t)vm->here;
    vm->here += sizeof(ptrdiff_t);
    *(forth_function_t*)vm->here = forth_while_runtime;
    vm->here += sizeof(ptrdiff_t);
}
static void forth_repeat_runtime(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    }
    ptrdiff_t* ret_addr = (ptrdiff_t*)*(vm->sp++);
    vm->pc = ret_addr;
}
void forth_repeat(forth_t* vm)
{
    ptrdiff_t* jmp_addr = (ptrdiff_t*)*(vm->sp++);
    *(forth_function_t*)vm->here = forth_push;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = *(vm->sp++); /* ret_addr */
    vm->here += sizeof(ptrdiff_t);
    *(forth_function_t*)vm->here = forth_repeat_runtime;
    vm->here += sizeof(ptrdiff_t);
    *jmp_addr = (ptrdiff_t)vm->here;
}
void forth_recurse(forth_t* vm)
{
    forth_unit_t* unit = vm->current;
    *(forth_function_t*)vm->here = forth_push;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = (ptrdiff_t)unit->cfa;
    vm->here += sizeof(ptrdiff_t);
    *(forth_function_t*)vm->here = forth_execute;
    vm->here += sizeof(ptrdiff_t);
}
void forth_constant(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    } 
    forth_create(vm);
    forth_unit_t* unit = vm->current;
    ptrdiff_t* data_ptr = unit->cfa;
    data_ptr[1] = *(vm->sp++);
    data_ptr[2] = (ptrdiff_t)forth_exit;
}
void forth_variable(forth_t* vm)
{
    forth_create(vm);
    *(--vm->sp) = 0;
    forth_comma(vm);
}
void forth_paren_brace(forth_t* vm)
{
    char* save_ptr;
    char* token = strtok_r(&vm->source[vm->in],")",&save_ptr);
    if(token != NULL)
    {
        vm->in += (save_ptr - &vm->source[vm->in]);
    }
    return;
}
void forth_words(forth_t* vm)
{
    forth_unit_t* p = vm->current;
    while(p != NULL)
    {
        *(--vm->sp) = (ptrdiff_t)p->name;
        *(--vm->sp) = strlen(p->name);
        forth_type(vm);
        forth_space(vm);
        p = (forth_unit_t*)p->next;
    }
}
void forth_depth(forth_t* vm)
{
    *(--vm->sp) = (vm->sp_start - vm->sp);
}
void forth_decimal(forth_t* vm)
{
    vm->base = 10;
}
void forth_hex(forth_t* vm)
{
    vm->base = 16;
}
void forth_left_bracket(forth_t* vm)
{
    vm->state = FORTH_STATE_INTERPRET;
}
void forth_right_bracket(forth_t* vm)
{
    vm->state = FORTH_STATE_COMPILE;
}
void forth_literal(forth_t* vm)
{
    if(forth_check_sp_underflow(vm,1))
    {
        vm->status |= FORTH_STATUS_STACK_UNDERFLOW;
        return;
    } 
    *(forth_function_t*)vm->here = forth_push;
    vm->here += sizeof(ptrdiff_t);
    *(ptrdiff_t*)vm->here = *(vm->sp++);
    vm->here += sizeof(ptrdiff_t);
}
void forth_postpone(forth_t* vm)
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
            break;
        }
        p = (forth_unit_t*)p->next;
    }
    if(p == NULL)
    {
        vm->status |= FORTH_STATUS_NOTFOUND;
        return;
    }
    forth_align(vm);
    *(ptrdiff_t**)vm->here = (ptrdiff_t*)*p->cfa;
    vm->here += sizeof(ptrdiff_t);
}

FORTH_FUNC_REG(forth_exit_unit,FORTH_FLAGS_IMMEDIATE,forth_exit_compile,"EXIT",NULL);
FORTH_FUNC_REG(forth_create_unit,0,forth_create,"CREATE",&forth_exit_unit);
FORTH_FUNC_REG(forth_find_unit,0,forth_find,"FIND",&forth_create_unit);
FORTH_FUNC_REG(forth_does_unit,FORTH_FLAGS_IMMEDIATE,forth_does,"DOES>",&forth_find_unit);
FORTH_FUNC_REG(forth_align_unit,0,forth_align,"ALIGN",&forth_does_unit);
FORTH_FUNC_REG(forth_aligned_unit,0,forth_aligned,"ALIGNED",&forth_align_unit);
FORTH_FUNC_REG(forth_cells_unit,0,forth_cells,"CELLS",&forth_aligned_unit);
FORTH_FUNC_REG(forth_cells_plus_unit,0,forth_cell_plus,"CELL+",&forth_cells_unit);
FORTH_FUNC_REG(forth_allot_unit,0,forth_allot,"ALLOT",&forth_cells_plus_unit);
FORTH_FUNC_REG(forth_move_unit,0,forth_move,"MOVE",&forth_allot_unit);
FORTH_FUNC_REG(forth_comma_unit,0,forth_comma,",",&forth_move_unit);
FORTH_FUNC_REG(forth_tick_unit,0,forth_tick,"'",&forth_comma_unit);
FORTH_FUNC_REG(forth_execute_unit,0,forth_execute,"EXECUTE",&forth_tick_unit);
FORTH_FUNC_REG(forth_plus_unit,0,forth_plus,"+",&forth_execute_unit);
FORTH_FUNC_REG(forth_minus_unit,0,forth_minus,"-",&forth_plus_unit);
FORTH_FUNC_REG(forth_mul_unit,0,forth_mul,"*",&forth_minus_unit);
FORTH_FUNC_REG(forth_xor_unit,0,forth_xor,"XOR",&forth_mul_unit);
FORTH_FUNC_REG(forth_over_unit,0,forth_over,"OVER",&forth_xor_unit);
FORTH_FUNC_REG(forth_lshift_unit,0,forth_lshift,"LSHIFT",&forth_over_unit);
FORTH_FUNC_REG(forth_rshift_unit,0,forth_rshift,"RSHIFT",&forth_lshift_unit);
FORTH_FUNC_REG(forth_mod_unit,0,forth_mod,"MOD",&forth_rshift_unit);
FORTH_FUNC_REG(forth_div_unit,0,forth_div,"/",&forth_mod_unit);
FORTH_FUNC_REG(forth_min_unit,0,forth_min,"MIN",&forth_div_unit);
FORTH_FUNC_REG(forth_max_unit,0,forth_max,"MAX",&forth_min_unit);
FORTH_FUNC_REG(forth_invert_unit,0,forth_invert,"INVERT",&forth_max_unit);
FORTH_FUNC_REG(forth_negate_unit,0,forth_negate,"NEGATE",&forth_invert_unit);
FORTH_FUNC_REG(forth_abs_unit,0,forth_abs,"ABS",&forth_negate_unit);
FORTH_FUNC_REG(forth_dup_unit,0,forth_dup,"DUP",&forth_abs_unit);
FORTH_FUNC_REG(forth_question_dup_unit,0,forth_question_dup,"?DUP",&forth_dup_unit);
FORTH_FUNC_REG(forth_drop_unit,0,forth_drop,"DROP",&forth_question_dup_unit);
FORTH_FUNC_REG(forth_swap_unit,0,forth_swap,"SWAP",&forth_drop_unit);
FORTH_FUNC_REG(forth_less_unit,0,forth_less,"<",&forth_swap_unit);
FORTH_FUNC_REG(forth_greater_unit,0,forth_greater,">",&forth_less_unit);
FORTH_FUNC_REG(forth_equal_unit,0,forth_equal,"=",&forth_greater_unit);
FORTH_FUNC_REG(forth_and_unit,0,forth_and,"AND",&forth_equal_unit);
FORTH_FUNC_REG(forth_or_unit,0,forth_or,"OR",&forth_and_unit);
FORTH_FUNC_REG(forth_zero_less_unit,0,forth_zero_less,"0<",&forth_or_unit);
FORTH_FUNC_REG(forth_zero_equal_unit,0,forth_zero_equal,"0=",&forth_zero_less_unit);
FORTH_FUNC_REG(forth_one_plus_unit,0,forth_one_plus,"1+",&forth_zero_equal_unit);
FORTH_FUNC_REG(forth_one_minus_unit,0,forth_one_minus,"1-",&forth_one_plus_unit);
FORTH_FUNC_REG(forth_colon_unit,0,forth_colon,":",&forth_one_minus_unit);
FORTH_FUNC_REG(forth_semicolon_unit,FORTH_FLAGS_IMMEDIATE,forth_semicolon,";",&forth_colon_unit);
FORTH_FUNC_REG(forth_store_unit,0,forth_store,"!",&forth_semicolon_unit);
FORTH_FUNC_REG(forth_fetch_unit,0,forth_fetch,"@",&forth_store_unit);
FORTH_FUNC_REG(forth_plus_store_unit,0,forth_plus_store,"+!",&forth_fetch_unit);
FORTH_FUNC_REG(forth_c_fetch_unit,0,forth_с_fetch,"C@",&forth_plus_store_unit);
FORTH_FUNC_REG(forth_c_store_unit,0,forth_с_store,"C!",&forth_c_fetch_unit);
FORTH_FUNC_REG(forth_c_comma_unit,0,forth_с_comma,"C,",&forth_c_store_unit);
FORTH_FUNC_REG(forth_char_unit,0,forth_сhar,"CHAR",&forth_c_comma_unit);
FORTH_FUNC_REG(forth_bracket_char_unit,FORTH_FLAGS_IMMEDIATE,forth_bracket_сhar,"[CHAR]",&forth_char_unit);
FORTH_FUNC_REG(forth_char_plus_unit,0,forth_сhar_plus,"CHAR+",&forth_bracket_char_unit);
FORTH_FUNC_REG(forth_chars_unit,0,forth_сhars,"CHARS",&forth_char_plus_unit);
FORTH_FUNC_REG(forth_squote_imm_unit,FORTH_FLAGS_IMMEDIATE,forth_squote,"S\"",&forth_chars_unit);
FORTH_FUNC_REG(forth_squote_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_squote,"S\"",&forth_squote_imm_unit);
FORTH_FUNC_REG(forth_dot_quote_unit,FORTH_FLAGS_IMMEDIATE | FORTH_FLAGS_COMPILE_ONLY,forth_dot_quote,".\"",&forth_squote_unit);
FORTH_FUNC_REG(forth_emit_unit,0,forth_emit,"EMIT",&forth_dot_quote_unit);
FORTH_FUNC_REG(forth_cr_unit,0,forth_cr,"CR",&forth_emit_unit);
FORTH_FUNC_REG(forth_space_unit,0,forth_space,"SPACE",&forth_cr_unit);
FORTH_FUNC_REG(forth_bl_unit,0,forth_bl,"BL",&forth_space_unit);
FORTH_FUNC_REG(forth_type_unit,0,forth_type,"TYPE",&forth_bl_unit);
FORTH_FUNC_REG(forth_true_unit,0,forth_true,"TRUE",&forth_type_unit);
FORTH_FUNC_REG(forth_false_unit,0,forth_false,"FALSE",&forth_true_unit);
FORTH_FUNC_REG(forth_here_unit,0,forth_here,"HERE",&forth_false_unit);
FORTH_FUNC_REG(forth_pad_unit,0,forth_pad,"PAD",&forth_here_unit);
FORTH_FUNC_REG(forth_hld_unit,0,forth_hld,"HLD",&forth_pad_unit);
FORTH_FUNC_REG(forth_less_number_sign_unit,0,forth_less_number_sign,"<#",&forth_hld_unit);
FORTH_FUNC_REG(forth_number_sign_unit,0,forth_number_sign,"#",&forth_less_number_sign_unit);
FORTH_FUNC_REG(forth_number_sign_s_unit,0,forth_number_sign_s,"#S",&forth_number_sign_unit);
FORTH_FUNC_REG(forth_number_sign_greater_unit,0,forth_number_sign_greater,"#>",&forth_number_sign_s_unit);
FORTH_FUNC_REG(forth_sign_unit,0,forth_sign,"SIGN",&forth_number_sign_greater_unit);
FORTH_FUNC_REG(forth_dot_unit,0,forth_dot,".",&forth_sign_unit);
FORTH_FUNC_REG(forth_current_unit,0,forth_current,"CURRENT",&forth_dot_unit);
FORTH_FUNC_REG(forth_base_unit,0,forth_base,"BASE",&forth_current_unit);
FORTH_FUNC_REG(forth_decimal_unit,0,forth_decimal,"DECIMAL",&forth_base_unit);
FORTH_FUNC_REG(forth_hex_unit,0,forth_hex,"HEX",&forth_decimal_unit);
FORTH_FUNC_REG(forth_immediate_unit,0,forth_immediate,"IMMEDIATE",&forth_hex_unit);
FORTH_FUNC_REG(forth_if_unit,FORTH_FLAGS_IMMEDIATE | FORTH_FLAGS_COMPILE_ONLY,forth_if,"IF",&forth_immediate_unit);
FORTH_FUNC_REG(forth_then_unit,FORTH_FLAGS_IMMEDIATE | FORTH_FLAGS_COMPILE_ONLY,forth_then,"THEN",&forth_if_unit);
FORTH_FUNC_REG(forth_else_unit,FORTH_FLAGS_IMMEDIATE | FORTH_FLAGS_COMPILE_ONLY,forth_else,"ELSE",&forth_then_unit);
FORTH_FUNC_REG(forth_do_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE ,forth_do,"DO",&forth_else_unit);
FORTH_FUNC_REG(forth_loop_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_loop,"LOOP",&forth_do_unit);
FORTH_FUNC_REG(forth_plus_loop_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_plus_loop,"+LOOP",&forth_loop_unit);
FORTH_FUNC_REG(forth_leave_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_leave,"LEAVE",&forth_plus_loop_unit);
FORTH_FUNC_REG(forth_unloop_unit,FORTH_FLAGS_COMPILE_ONLY,forth_unloop,"UNLOOP",&forth_leave_unit);
FORTH_FUNC_REG(forth_i_unit,0,forth_i,"I",&forth_unloop_unit);
FORTH_FUNC_REG(forth_j_unit,0,forth_j,"J",&forth_i_unit);
FORTH_FUNC_REG(forth_begin_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_begin,"BEGIN",&forth_j_unit);
FORTH_FUNC_REG(forth_until_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_until,"UNTIL",&forth_begin_unit);
FORTH_FUNC_REG(forth_while_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_while,"WHILE",&forth_until_unit);
FORTH_FUNC_REG(forth_repeat_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_repeat,"REPEAT",&forth_while_unit);
FORTH_FUNC_REG(forth_recurse_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_recurse,"RECURSE",&forth_repeat_unit);
FORTH_FUNC_REG(forth_constant_unit,0,forth_constant,"CONSTANT",&forth_recurse_unit);
FORTH_FUNC_REG(forth_variable_unit,0,forth_variable,"VARIABLE",&forth_constant_unit);
FORTH_FUNC_REG(forth_words_unit,0,forth_words,"WORDS",&forth_variable_unit);
FORTH_FUNC_REG(forth_depth_init,0,forth_depth,"DEPTH",&forth_words_unit);
FORTH_FUNC_REG(forth_paren_brace_unit,FORTH_FLAGS_IMMEDIATE,forth_paren_brace,"(",&forth_depth_init);
FORTH_FUNC_REG(forth_left_bracket_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_left_bracket,"[",&forth_paren_brace_unit);
FORTH_FUNC_REG(forth_right_bracket_unit,0,forth_right_bracket,"]",&forth_left_bracket_unit);
FORTH_FUNC_REG(forth_literal_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_literal,"LITERAL",&forth_right_bracket_unit);
FORTH_FUNC_REG(forth_postpone_unit,FORTH_FLAGS_COMPILE_ONLY | FORTH_FLAGS_IMMEDIATE,forth_postpone,"POSTPONE",&forth_literal_unit);
FORTH_FUNC_REG(forth_unit,0,forth_exit,"FORTH",&forth_postpone_unit);
const forth_unit_t* FORTH_ADDR = &forth_unit;