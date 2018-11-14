cmake_minimum_required(VERSION 3.10)
project(MiniBug C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS)

include_directories(.)

add_executable(MiniBug khh.h mdb.c)
add_executable(MiniView khh.h mview.c)
