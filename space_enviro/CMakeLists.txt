cmake_minimum_required(VERSION 3.10)

project(space_enviro)
set(CMAKE_CXX_STANDARD 17)

add_library(env SHARED
        environment_controller.h
        environment_controller.cpp
        render_engine.h
        Entities/entities_data.h
        Entities/checkpoint.h
        Scenarios/scenario_base.h
        Scenarios/checkpoint_scenario.h
        Entities/basic_render_types.h
        global_config.h
        waiter.h
        )
target_include_directories(env PUBLIC /usr/include/python3.6)
target_link_libraries(
        env
        boost_python3-py36
        boost_numpy3-py36
        -lsfml-graphics
        -lsfml-window
        -lsfml-system
        )

add_custom_command(TARGET env POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy
        "libenv.so"
        "../spaceLib.so"
        COMMENT "Copying to output directory")