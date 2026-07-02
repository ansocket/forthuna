#include "forth.h"

void forth_ret(forth_t* vm)
{

} 
const forth_unit_t forth_cmd = 
{
    .flags = 0,
    .name = "+",
    .next = NULL,
    .data = {
        forth_ret
    }
};