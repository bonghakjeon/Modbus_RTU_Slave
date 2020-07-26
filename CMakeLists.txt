cmake_minimum_required(VERSION 3.16)
project(Mod_Slave C)

set(CMAKE_C_STANDARD 11)

add_executable(Mod_Slave slave.c mod.h mod.c)