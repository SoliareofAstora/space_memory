cmake_minimum_required(VERSION 3.10)

project(space_enviro)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-W)

set(CMAKE_CXX_FLAGS "-lboost_python -I /home/overlord/miniconda3/envs/space_memory/include/python3.7m")
add_library(env SHARED main.cpp )

add_custom_command(TARGET env POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy
        "libenv.so"
        "../spaceLib.so"
        COMMENT "Copying to output directory")