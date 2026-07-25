add_library(custom_set custom_set.c)
target_link_libraries(custom_set PUBLIC forthuna)
target_include_directories(custom_set PUBLIC .)