#include "custom_set.h"
#include "forth.h"

#include "stdio.h"

void custom_func(forth_t* vm)
{
    printf("Hello from custom set!");
    *(--vm->sp) = 23;
}
FORTH_FUNC_RAM_REG(custom_func_unit,0,custom_func,"CUSTOM_FUNC",NULL); /* First func must be on RAM to have ability to modify "next" field */

void custom_func2(forth_t* vm)
{
    printf("Hello from custom set2!");
    *(--vm->sp) = 84;
}
FORTH_FUNC_REG(custom_func2_unit,0,custom_func2,"CUSTOM_FUNC2",&custom_func_unit);

const forth_unit_t* custom_set_start = &custom_func2_unit;
forth_unit_t* custom_set_bridge = &custom_func_unit;