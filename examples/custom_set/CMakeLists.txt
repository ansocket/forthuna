add_subdirectory(custom_set)

add_executable(custom_set_app main.c)
target_link_libraries(custom_set_app PRIVATE forthuna custom_set)
target_compile_options(custom_set_app PRIVATE -fmessage-length=0 
                                    -ffunction-sections 
                                    -fdata-sections )
target_link_options(custom_set_app PRIVATE -Wl,-Map=${CMAKE_PROJECT_NAME}.map 
                        -Wl,--gc-sections )