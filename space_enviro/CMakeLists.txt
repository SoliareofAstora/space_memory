cmake_minimum_required(VERSION 3.10)

project(space_enviro)
set(CMAKE_CXX_STANDARD 17)

add_library(env SHARED main.cpp )
target_include_directories(env PUBLIC /usr/include/python3.6)
target_link_libraries(env boost_python3-py36 -lsfml-graphics -lsfml-window -lsfml-system )

add_custom_command(TARGET env POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy
        "libenv.so"
        "../spaceLib.so"
        COMMENT "Copying to output directory")