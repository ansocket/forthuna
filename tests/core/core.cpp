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
// TEST(forth_core, if_then) {
//   forth vm;
//   ptrdiff_t* memory = (ptrdiff_t*)malloc(4096);
//   forth_init_t init_desc = 
//   {
//     .full_memory = memory,
//     .full_memory_cells = 4096/sizeof(ptrdiff_t),
//     .sp_cells = 32, 
//     .rsp_cells = 32,
//     .tib_bytes = 128
//   };
//   forth_init(&vm,&init_desc);
//   forth_reg_input_cb(&vm,[]() -> char {
//     // char res =  getchar();
//     // if(res == '\n') 
//     // {
//     //   res = '\0';
//     // }
//     // return res;
//     static int i = 0; 
//     const char* input_string = ": BEBE IF 10 + THEN ; 20 TRUE BEBE FALSE BEBE";
//     return input_string[i++];
//         });
//   forth_process(&vm);
//   EXPECT_EQ(*vm.sp++,30);
//   EXPECT_EQ(vm.sp,vm.sp_start);
//   free(memory);
// }
