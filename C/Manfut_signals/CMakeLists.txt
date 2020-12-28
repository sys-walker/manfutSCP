cmake_minimum_required(VERSION 3.17)
project(Manfut_signals C)

set(CMAKE_C_STANDARD 99)

add_executable(Manfut Manfut.c)
target_link_libraries (Manfut pthread )
target_link_libraries (Manfut m)
#add_compile_options(-Wall -Wextra -pedantic -Werror)
add_compile_options( -ansi -pedantic-errors)