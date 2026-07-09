#include "gtest/gtest.h"
#include "forth.h"

namespace {
TEST(forth_core, Init) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
  EXPECT_EQ(vm.sp, vm.sp_start);
  free(memory);
}
TEST(forth_core, simple) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "1 2 +";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,3);
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
}
TEST(forth_core, execute) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, colon) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, c_variable) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "VAR1 @ VAR2 @ 22 VAR2 ! 33 VAR1 ! VAR1 @ VAR2 @";
    return input_string[i++];
        });
    forth_add_variable(&vm,"VAR1",10);
    forth_add_variable(&vm,"VAR2",20);
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,22);
  EXPECT_EQ(*vm.sp++,33);
  EXPECT_EQ(*vm.sp++,20);
  EXPECT_EQ(*vm.sp++,10);
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, create_does) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  free(memory);
}
TEST(forth_core, create_does_var) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, if_then) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  free(memory);
}
TEST(forth_core, if_then_else) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, do_loop) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, do_loop_nested) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, do_plus_loop) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(*vm.sp++, -1);
  EXPECT_EQ(*vm.sp++, 0);
  EXPECT_EQ(*vm.sp++, 1);
  EXPECT_EQ(*vm.sp++, 2);
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, do_loop_leave) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, dup) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, logic) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " 10 15 < 88 105 > 88 33 = 55 27 > 44 44 =";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_FALSE);
  EXPECT_EQ(*vm.sp++, FORTH_TRUE);
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, recurse) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
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
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
TEST(forth_core, swap) {
  forth vm;
  ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
  forth_init_t init_desc = 
  {
    .full_memory = memory,
    .full_memory_cells = 4096/sizeof(ptrdiff_t),
    .sp_cells = 32, 
    .rsp_cells = 32,
    .tib_bytes = 128
  };
  forth_init(&vm,&init_desc);
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = " 10 20 SWAP";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++, 10);
  EXPECT_EQ(*vm.sp++, 20);
  EXPECT_EQ(vm.sp,vm.sp_start);
  free(memory);
}
