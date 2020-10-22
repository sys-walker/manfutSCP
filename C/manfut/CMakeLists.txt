cmake_minimum_required(VERSION 3.17)
project(manfut C)

set(CMAKE_C_STANDARD 99)

add_executable(manfut manfut.c)
target_link_libraries (manfut m)


add_executable(ConcManfut ConcManfut.c)
target_link_libraries (ConcManfut m)
target_link_libraries (ConcManfut pthread)