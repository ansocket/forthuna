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
    this->memory = (ptrdiff_t*)malloc(4096);
    forth_init_t init_desc = 
    {
      .full_memory = this->memory,
      .full_memory_cells = 4096/sizeof(ptrdiff_t),
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
    free(this->memory);
  }

  ptrdiff_t* memory;
  forth vm;
};


TEST_F(CoreTests, Init) {

  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp, vm.sp_start);
}
TEST_F(CoreTests, numbers) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "1 2";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,2);
  EXPECT_EQ(*vm.sp++,1);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

TEST_F(CoreTests, execute) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "1 2 ' + EXECUTE";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,3);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, ariph) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " + \n 1 2 + \n - \n 1 2 - \n * \n 30 40 *";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " 10 / \0 0 / \0 DROP 5 /";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " ROT \n 1 2 3 ROT";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " OVER \n 1 2 OVER";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " LSHIFT \n RSHIFT \n 2 4 LSHIFT 155 2 RSHIFT";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " XOR \n 10 20 XOR 20 10 XOR";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,20 ^ 10);
  EXPECT_EQ(*vm.sp++,10 ^ 20);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, mod) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " MOD \n 10 0 MOD \n DROP DROP 10 20 MOD 20 10 MOD 20 3 MOD";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "MIN \n MAX \n 10 20 MIN 20 10 MAX -20 1 MAX -20 30 MIN";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "INVERT \n NEGATE \n ABS \n 43 INVERT 57 NEGATE -4 NEGATE -4747 ABS 4374 ABS";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "CONSTANT \n 10 CONSTANT \n CONSTANT jojo jojo";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "VARIABLE \n VARIABLE jojo 20 jojo ! jojo @";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_INPUT_ERROR);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,20);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, colon) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": BEBE 1 2 + ; BEBE : KOKO 4 BEBE + ; ' KOKO EXECUTE +";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, c_variable) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "VAR1 @ VAR2 @ 22 VAR2 ! 33 VAR1 ! VAR1 @ VAR2 @ \n @ \n !";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "BEBE \n BEBE";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "BEBE \n BEBE @ 10 BEBE !";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "BEBE \n BEBE";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": CUST_CONST CREATE , DOES> @ ; 10 CUST_CONST KOKO KOKO ";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, create_does_var) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": CUST_VAR CREATE 0 , ; CUST_VAR KOKO 10 KOKO ! KOKO @ ";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, if_then) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": IF_TEST IF 10 + THEN ; 20 TRUE IF_TEST FALSE IF_TEST";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,30);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, if_then_else) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": IF_ELSE_TEST IF 10 99 + ELSE 20 EXIT + THEN ; TRUE IF_ELSE_TEST FALSE IF_ELSE_TEST";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,20);
  EXPECT_EQ(*vm.sp++,109);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, do_loop) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": DO_LOOP_TEST 10 0 DO I LOOP ; DO_LOOP_TEST";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": GD3 DO 1 0 DO J LOOP LOOP ; 4 1 GD3";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,3);
  EXPECT_EQ(*vm.sp++,2);
  EXPECT_EQ(*vm.sp++,1);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, do_plus_loop) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": GD2 DO I -1 +LOOP ; -1 2 GD2";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": GD3 DO I 2 +LOOP ; 6 2 GD3";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(*vm.sp++, 2);

  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, do_loop_leave) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": GD2 10 0 DO -5 I + IF I ELSE LEAVE 99 THEN 10 LOOP ; GD2";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " 10 15 DUP ";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 15);
  EXPECT_EQ(*vm.sp++, 15);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, logic) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " 10 15 < 88 105 > 88 33 = 55 27 > 44 44 = TRUE TRUE AND FALSE TRUE OR TRUE FALSE AND FALSE FALSE OR \n \
                                < \n > \n = \n AND \n OR";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " : FACTORIAL DUP 2 < IF DROP 1 EXIT THEN  DUP 1 - RECURSE * ; 4 FACTORIAL";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 24);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, swap) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " 10 20 SWAP \n SWAP";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " : STR_TEST S\" abcdefjh\" SWAP C@ ; STR_TEST S\" ab\" SWAP C@";
    return input_string[i++];
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " : GC4 S\" XY\" ; GC4 SWAP DROP GC4 DROP DUP C@ SWAP CHAR+ C@";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 89);
  EXPECT_EQ(*vm.sp++, 88);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, emit) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "10 89 88 EMIT EMIT EMIT \n EMIT";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
  });
  forth_process(&vm);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
  forth_process(&vm);
  EXPECT_EQ(vm.status,FORTH_STATUS_STACK_UNDERFLOW);
}
TEST_F(CoreTests, type) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "S\" bebebe\" TYPE CR \n TYPE \n 17 TYPE";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "<# 10 0 # # #> TYPE 16 BASE ! \
                                      <# FF 0 # # #> TYPE \
                                      <# 442 0 #S #> TYPE \
                                      A BASE ! \
                                      <# 15 0 #S -1 SIGN #> TYPE \
                                      10 EMIT";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
  });
  forth_process(&vm);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, dot) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "10 20 + . 10 EMIT";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
  });
  forth_process(&vm);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

TEST_F(CoreTests, multiple_runs) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "10 20 + DUP . \0 : BEBE 40 DUP . + \0 ; BEBE DUP .";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
  });
  forth_process(&vm);
  forth_process(&vm);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 70);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}
TEST_F(CoreTests, begin_while) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": GI4 BEGIN DUP 1 + DUP 5 > UNTIL ; 3 GI4";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": GI3 BEGIN DUP 5 < WHILE DUP 1 + REPEAT ; 0 GI3 4 GI3 5 GI3 6 GI3";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
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
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": lit_test 1 [ 2 3 + ] LITERAL + ; lit_test";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
  });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 6);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

TEST_F(CoreTests, postpone) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": ENDIF POSTPONE THEN ; IMMEDIATE 99 FALSE : TEST IF 1 2 + ENDIF ; TEST";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
  });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 99);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

TEST_F(CoreTests, unloop) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = ": GD6 ( PAT: {0 0},{0 0}{1 0}{1 1},{0 0}{1 0}{1 1}{2 0}{2 1}{2 2} ) \
      0 SWAP 0 DO \
         I 1+ 0 DO \
           I J + 3 = IF I UNLOOP I UNLOOP EXIT THEN 1+ \
         LOOP \
      LOOP ; 3 GD6 ";
    return input_string[i++];
        });
  forth_reg_output_cb(&vm,[](char ch) -> void {
    std::putchar(ch);
  });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 4);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

}