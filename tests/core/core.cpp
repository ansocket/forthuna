#include "gtest/gtest.h"
#include "forth.h"

namespace {

class CoreTests : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.
  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the variables.
  // Otherwise, this can be skipped.
  void SetUp() override {
    this->memory = new ptrdiff_t[4096/sizeof(ptrdiff_t)];
    this->stack = new ptrdiff_t[32];
    this->rstack = new ptrdiff_t[32];
    forth_init_t init_desc = 
    {
      .memory = this->memory,
      .stack = this->stack,
      .rstack = this->rstack,
      .memory_cells = 4096/sizeof(ptrdiff_t),
      .sp_cells = 32, 
      .rsp_cells = 32,
      .tib_bytes = 1024
    };
    forth_init(&vm,&init_desc);
  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  virtual void TearDown() {
    delete [] this->memory;
    delete [] this->stack;
    delete [] this->rstack;
    
  }

  // ptrdiff_t memory[4096/sizeof(ptrdiff_t)];
  // ptrdiff_t stack[32];
  // ptrdiff_t rstack[32];
  ptrdiff_t* memory;
  ptrdiff_t* stack;
  ptrdiff_t* rstack;
  forth vm;
};


TEST_F(CoreTests, Init) {

  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp, vm.sp_start);
}
TEST_F(CoreTests, numbers) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    const char* input_string = "DROP \0 1 2";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,2);
  EXPECT_EQ(*vm.sp++,1);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

TEST_F(CoreTests, execute) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "1 2 ' + EXECUTE";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,3);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, ariph) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " + \0 1 2 + \0 - \0 1 2 - \0 * \0 30 40 *";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,3);
  EXPECT_EQ(vm.status,0);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,-1);
  EXPECT_EQ(vm.status,0);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,1200);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, zero_div) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " 10 / \0 0 / \0 DROP 5 /";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_ZERO_DIVISION);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,2);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, rot) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " ROT \0 1 2 3 ROT";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,1);
  EXPECT_EQ(*vm.sp++,3);
  EXPECT_EQ(*vm.sp++,2);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, over) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " OVER \0 1 2 OVER";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,1);
  EXPECT_EQ(*vm.sp++,2);
  EXPECT_EQ(*vm.sp++,1);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, shifts) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " LSHIFT \0 RSHIFT \0 2 4 LSHIFT 155 2 RSHIFT";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,155 >> 2);
  EXPECT_EQ(*vm.sp++,2 << 4);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, xor) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " XOR \0 10 20 XOR 20 10 XOR";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,20 ^ 10);
  EXPECT_EQ(*vm.sp++,10 ^ 20);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, mod) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " MOD \0 10 0 MOD \0 DROP DROP 10 20 MOD 20 10 MOD 20 3 MOD";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_ZERO_DIVISION);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,20 % 3);
  EXPECT_EQ(*vm.sp++,20 % 10);
  EXPECT_EQ(*vm.sp++,10 % 20);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, min_max) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "MIN \0 MAX \0 10 20 MIN 20 10 MAX -20 1 MAX -20 30 MIN";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,-20);
  EXPECT_EQ(*vm.sp++,1);
  EXPECT_EQ(*vm.sp++,20);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, invert_negate_abs) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "INVERT \0 NEGATE \0 ABS \0 43 INVERT 57 NEGATE -4 NEGATE -4747 ABS 4374 ABS";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,4374);
  EXPECT_EQ(*vm.sp++,4747);
  EXPECT_EQ(*vm.sp++,4);
  EXPECT_EQ(*vm.sp++,-57);
  EXPECT_EQ(*vm.sp++,~43);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, constant) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "CONSTANT \0 10 CONSTANT \0 CONSTANT jojo jojo";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_INPUT_ERROR);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, variable) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "VARIABLE \0 VARIABLE jojo 20 jojo ! jojo @";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_INPUT_ERROR);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,20);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, colon) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": BEBE 1 2 + ; BEBE : KOKO 4 BEBE + ; ' KOKO EXECUTE +";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, c_variable) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "VAR1 @ VAR2 @ 22 VAR2 ! 33 VAR1 ! VAR1 @ VAR2 @ \0 @ \0 !";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
    forth_add_variable(&vm,"VAR1",10);
    forth_add_variable(&vm,"VAR2",20);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,22);
  EXPECT_EQ(*vm.sp++,33);
  EXPECT_EQ(*vm.sp++,20);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
}
TEST_F(CoreTests, c_func) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "BEBE \0 BEBE";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_NOTFOUND);
  forth_add_function(&vm, "BEBE", [](forth_t* vm) -> void
{
  *(--vm->sp) = 99;
}, 0);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,99);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, c_var_ptr) {
  ptrdiff_t variable = 20;
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "BEBE \0 BEBE @ 10 BEBE !";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_NOTFOUND);
  forth_add_variable_ptr(&vm, "BEBE", &variable);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,20);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
  EXPECT_EQ(variable,10);
}
TEST_F(CoreTests, c_const) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "BEBE \0 BEBE";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_NOTFOUND);
  forth_add_constant(&vm, "BEBE", 10);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, create_does) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": CUST_CONST CREATE , DOES> @ ; 10 CUST_CONST KOKO KOKO ";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, create_does_var) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": CUST_VAR CREATE 0 , ; CUST_VAR KOKO 10 KOKO ! KOKO @ ";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, if_then) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": IF_TEST IF 10 + THEN ; 20 TRUE IF_TEST FALSE IF_TEST";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,30);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, if_then_else) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": IF_ELSE_TEST IF 10 99 + ELSE 20 EXIT + THEN ; TRUE IF_ELSE_TEST FALSE IF_ELSE_TEST";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,20);
  EXPECT_EQ(*vm.sp++,109);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, if_then_nested) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": IF_NESTED IF IF 10 ELSE 20 THEN ELSE IF 30 ELSE 40 THEN THEN ; FALSE FALSE IF_NESTED TRUE FALSE IF_NESTED FALSE TRUE IF_NESTED TRUE TRUE IF_NESTED";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(*vm.sp++,20);
  EXPECT_EQ(*vm.sp++,30);
  EXPECT_EQ(*vm.sp++,40);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, do_loop) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": DO_LOOP_TEST 10 0 DO I LOOP ; DO_LOOP_TEST";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,9);
  EXPECT_EQ(*vm.sp++,8);
  EXPECT_EQ(*vm.sp++,7);
  EXPECT_EQ(*vm.sp++,6);
  EXPECT_EQ(*vm.sp++,5);
  EXPECT_EQ(*vm.sp++,4);
  EXPECT_EQ(*vm.sp++,3);
  EXPECT_EQ(*vm.sp++,2);
  EXPECT_EQ(*vm.sp++,1);
  EXPECT_EQ(*vm.sp++,0);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, do_loop_nested) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": GD3 DO 1 0 DO J LOOP LOOP ; 4 1 GD3";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,3);
  EXPECT_EQ(*vm.sp++,2);
  EXPECT_EQ(*vm.sp++,1);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, do_plus_loop) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": GD2 DO I -1 +LOOP ; -1 2 GD2";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,-1);
  EXPECT_EQ(*vm.sp++, 0);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 2);

  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, do_plus_loop1) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": GD3 DO I 2 +LOOP ; 6 2 GD3";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(*vm.sp++, 2);

  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, do_loop_leave) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": GD2 10 0 DO -5 I + IF I ELSE LEAVE 99 THEN 10 LOOP ; GD2";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(*vm.sp++, 3);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(*vm.sp++, 0);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, dup) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " DUP \0 ?DUP \0 10 15 DUP 0 ?DUP 12 ?DUP";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 12);
  EXPECT_EQ(*vm.sp++, 12);
  EXPECT_EQ(*vm.sp++, 0);
  EXPECT_EQ(*vm.sp++, 15);
  EXPECT_EQ(*vm.sp++, 15);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, logic) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " 10 15 < 88 105 > 88 33 = 55 27 > 44 44 = TRUE TRUE AND FALSE TRUE OR TRUE FALSE AND FALSE FALSE OR \0 \
                                < \0 > \0 = \0 AND \0 OR";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
}
TEST_F(CoreTests, recurse) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " : FACTORIAL DUP 2 < IF DROP 1 EXIT THEN  DUP 1 - RECURSE * ; 4 FACTORIAL";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 24);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, swap) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " 10 20 SWAP \0 SWAP";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(*vm.sp++, 20);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
}
TEST_F(CoreTests, sq) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " : STR_TEST S\" abcdefjh\" SWAP C@ ; STR_TEST S\" ab\" SWAP C@";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 97);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(*vm.sp++, 97);
  EXPECT_EQ(*vm.sp++, 8);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, sq1) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " : GC4 S\" XY\" ; GC4 SWAP DROP GC4 DROP DUP C@ SWAP CHAR+ C@";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 89);
  EXPECT_EQ(*vm.sp++, 88);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, emit) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "10 89 88 EMIT EMIT EMIT \0 EMIT";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
}
TEST_F(CoreTests, type) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "S\" bebebe\" TYPE CR \0 TYPE \0 17 TYPE";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
}
TEST_F(CoreTests, formatting) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "<# 10 0 # # #> TYPE 16 BASE ! \
                                      <# FF 0 # # #> TYPE \
                                      <# 442 0 #S #> TYPE \
                                      A BASE ! \
                                      <# 15 0 #S -1 SIGN #> TYPE \
                                      10 EMIT";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, dot) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "10 20 + . 10 EMIT";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

TEST_F(CoreTests, multiple_runs) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "10 20 + DUP . \0 : BEBE 40 DUP . + \0 ; BEBE DUP .";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  forth_process(&vm);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 70);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, begin_while) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": GI4 BEGIN DUP 1 + DUP 5 > UNTIL ; 3 GI4";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 6);
  EXPECT_EQ(*vm.sp++, 5);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(*vm.sp++, 3);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, begin_while_repeat) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": GI3 BEGIN DUP 5 < WHILE DUP 1 + REPEAT ; 0 GI3 4 GI3 5 GI3 6 GI3";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 6);
  EXPECT_EQ(*vm.sp++, 5);
  EXPECT_EQ(*vm.sp++, 5);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(*vm.sp++, 5);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(*vm.sp++, 3);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 0);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, literal) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": wrong 1 [ ] LITERAL \0 ; : lit_test 1 [ 2 3 + ] LITERAL + ; lit_test";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 6);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

TEST_F(CoreTests, postpone) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "POSTPONE \0 : bebe POSTPONE \0 POSTPONE keke \0 ; : ENDIF POSTPONE THEN ; IMMEDIATE 99 FALSE : TEST IF 1 2 + ENDIF ; TEST";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_NOTFOUND);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_INPUT_ERROR);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_NOTFOUND);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 99);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

TEST_F(CoreTests, unloop) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": GD6 ( PAT: {0 0},{0 0}{1 0}{1 1},{0 0}{1 0}{1 1}{2 0}{2 1}{2 2} ) \
      0 SWAP 0 DO \
         I 1+ 0 DO \
           I J + 3 = IF I UNLOOP I UNLOOP EXIT THEN 1+ \
         LOOP \
      LOOP ; 3 GD6 ";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, marker) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " HERE @ CURRENT @ MARKER M1 : JUNK1 1 2 + ; : JUNK2 4 2 4 3 ; M1 HERE @ CURRENT @";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  ptrdiff_t current2 = *vm.sp++;
  ptrdiff_t here2 = *vm.sp++;
  EXPECT_EQ(*vm.sp++, current2);
  EXPECT_EQ(*vm.sp++, here2);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, s_to_d) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " S>D \0 100 S>D  ";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 0);
  EXPECT_EQ(*vm.sp++, 100);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, two_store_fetch) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "  2! \0 2@ \0 VARIABLE TEST_2SF 100 S>D TEST_2SF 2! TEST_2SF 2@";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 0);
  EXPECT_EQ(*vm.sp++, 100);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, two_dup_swap_over) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = " 2SWAP \0 2DUP \0 2OVER \0 2DROP \0 1 2 3 4 2SWAP 5 6 2DUP 1 2 3 4 2OVER 3 2 2DROP";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(*vm.sp++, 3);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 6);
  EXPECT_EQ(*vm.sp++, 5);
  EXPECT_EQ(*vm.sp++, 6);
  EXPECT_EQ(*vm.sp++, 5);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(*vm.sp++, 3);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, accept) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "ACCEPT \0 VARIABLE TEST_ACCEPT 10 ALLOT TEST_ACCEPT 10 ACCEPT \0 12345\0 + 0 SWAP C! TEST_ACCEPT";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  forth_process(&vm);
  EXPECT_STREQ((char*)*vm.sp++, " 12345");
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, base) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "HEX BASE @ FF DECIMAL BASE @ 53";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 53);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(*vm.sp++, 255);
  EXPECT_EQ(*vm.sp++, 16);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, depth) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "DEPTH 1 2 3 DEPTH";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(*vm.sp++, 3);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 0);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, zero_less_and_co) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "0< \0 0= \0 1+ \0 1- \0 -5 0< 10 0< 0 0< 10 0= 0 0= 6 1+ 15 1-";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 14);
  EXPECT_EQ(*vm.sp++, 7);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, plus_store) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "+! \0 VARIABLE 1ST 0 1ST ! 1 1ST +! 1ST @ -1 1ST +! 1ST @";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 0);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, c_fetch_c_store) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "C@ \0 C! \0 : TEST_ST S\" XY\" DROP ; 80 TEST_ST C! TEST_ST C@";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 80);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, char_cmd) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "CHAR \0 CHAR BEBE";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_INPUT_ERROR);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 'B');
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, char_bracket_test) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ": wrong [CHAR] \0 ; : TEST [CHAR] HELLO ; TEST";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_INPUT_ERROR);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 'H');
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, c_comma_test) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "C, \0 : TEST_CC CREATE C, C, ; 80 81 TEST_CC KEKE KEKE C@ KEKE 1+ C@ ";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 80);
  EXPECT_EQ(*vm.sp++, 81);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, char_plus_test) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "CHAR+ \0 10 CHAR+";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 11);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, chars_test) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "CHARS \0 10 CHARS";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, cell_plus_test) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "CELL+ \0 10 CELL+";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 10 + sizeof(ptrdiff_t));
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, cells_test) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "CELLS \0 10 CELLS";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 10 * sizeof(ptrdiff_t));
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, move_test) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = "CMOVE \0 : TEST_STR S\" abcde\" ; VARIABLE CP_STR TEST_STR CP_STR SWAP DUP ALLOT CMOVE TEST_STR SWAP DROP CP_STR + 0 SWAP C! CP_STR";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_STREQ((char*)*vm.sp++, "abcde");
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, r_move_fetch) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    
    const char* input_string = ">R \0 10 >R R@ R>";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, u_less_test) {
  forth_reg_input_cb(&vm,[](forth_t* vm, char* data, int maxlen) -> int {
    // char res =  getchar();
    // if(res == '\0') 
    // {
    //   res = '\0';
    // }
    // return res;
    const char* input_string = "U< \0 -10 2368678 U< 329749832 93829 U< -1214 -18 U<";
    static int i = 0;
    strcpy(data,input_string + i);
    i += strlen(data) + 1;
    return strlen(data) + 1;
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
}