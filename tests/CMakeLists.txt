target_compile_options(forthuna PUBLIC --coverage)
target_link_options(forthuna PUBLIC --coverage)


add_subdirectory(core)
add_subdirectory(custom_set)