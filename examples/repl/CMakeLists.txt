add_executable(repl main.c)
target_link_libraries(repl PRIVATE forthuna)
target_compile_options(repl PRIVATE -fmessage-length=0 
                                    -ffunction-sections 
                                    -fdata-sections
                                    -Os )
target_link_options(repl PRIVATE -Wl,-Map=${CMAKE_PROJECT_NAME}.map 
                        -Wl,--gc-sections )
add_custom_command(TARGET repl POST_BUILD COMMAND size ARGS ${CMAKE_BINARY_DIR}/examples/repl/repl)