cmake_minimum_required(VERSION 3.10)

project(space_enviro)
set(CMAKE_CXX_STANDARD 17)

add_library(env SHARED
        environment_controller.cpp
        environment_controller.h
        Rendering/render_engine.h
        Entities/checkpoint.h
        Scenario/scenario_base.h
        Scenario/checkpoint_scenario.h
        global_config.h
        Utils/waiter.h
        Entities/ship_array.hpp
        Entities/Data/base_entity.h
        Entities/Data/dynamic_entity.h
        Entities/Data/ship.h
        Rendering/Primitives/render_ship.h
        Rendering/Primitives/render_square.hpp
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