#include "stdio.h"
#include "forth.h"

#include "termios.h"

static struct termios stored_settings;

void setup_terminal(void)
{
	struct termios new_settings;

	tcgetattr(0,&stored_settings);

	new_settings = stored_settings;

	new_settings.c_lflag &= (~ICANON & ~ECHO);
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 1;

	tcsetattr(0,TCSANOW,&new_settings);
	return;
}
char memory[4096];
ptrdiff_t stack[32];
ptrdiff_t rstack[32];
forth_t vm;

static int input_cb(forth_t* vm, char* data, int maxlen)
{
    int res = 0;
    while(1)
    {
        char ch = getchar();
        if(vm->local_echo != 0)
        {
            putchar(ch);
        }
        if((ch == '\n') || ((ch == '\0')))
        {
            ch = '\0';
            data[res++] = ch;
            break;
        }
        else if((ch == '\b') || (ch == '\x7F'))
        {
            if(res > 0)
            {
                res--;
                if(vm->local_echo != 0)
                {
                    putchar('\b');
                    putchar(' ');
                    putchar('\b');
                }
            }
            continue;
        }
        else
        {
            data[res] = ch;
        }
        res++;
        if(res >= maxlen)
        {
            break;
        }
    }
    return res;
} 
static void output_cb(forth_t* vm, const char* data, int len)
{
    for(int i = 0; i < len; i++)
    {
        putchar(data[i]);
    }
}
int main(int argc, char** argv)
{
    setup_terminal();
    forth_init_t init_desc = 
    {
        .memory = (ptrdiff_t*)memory,
        .memory_cells = 4096/sizeof(ptrdiff_t),
        .stack = stack,
        .rstack = rstack,
        .sp_cells = 32, 
        .rsp_cells = 32,
        .tib_bytes = 1024
    };
    forth_init(&vm,&init_desc);
    forth_reg_input_cb(&vm,input_cb);
    forth_reg_output_cb(&vm, output_cb);
    forth_send_report(&vm, "Forthuna REPL. Enjoy!\n");
    while(1)
    {
        forth_process(&vm);
        vm.status = 0;
    }
    return 0;
}