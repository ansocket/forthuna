#include "gtest/gtest.h"
#include "forth.h"
#include "custom_set.h"

namespace {

class CustomSetTests : public testing::Test {
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
    forth_add_dictionary(&vm,custom_set_start,custom_set_bridge);
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


TEST_F(CustomSetTests, CustomSet) {
  forth_reg_input_cb(&vm,[]() -> char {
    // char res =  getchar();
    // if(res == '\n') 
    // {
    //   res = '\0';
    // }
    // return res;
    static int i = 0; 
    const char* input_string = "CUSTOM_FUNC CUSTOM_FUNC2";
    return input_string[i++];
        });
  forth_process(&vm);
  EXPECT_EQ(*vm.sp++,84);
  EXPECT_EQ(*vm.sp++,23);
  EXPECT_EQ(vm.status,0);
  EXPECT_EQ(vm.sp,vm.sp_start);
}

}