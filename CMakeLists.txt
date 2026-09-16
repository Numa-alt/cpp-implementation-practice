cmake_minimum_required(VERSION 3.20)

project(cpp_implementation_practice LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task_01_task_manager
    task-01-task-manager/main.cpp
)

add_executable(task_02_party
    task-02-party/main.cpp
)

if(MSVC)
    target_compile_options(task_01_task_manager PRIVATE
        /W4
        /utf-8
    )
    target_compile_options(task_02_party PRIVATE
        /W4
        /utf-8
    )
else()
    target_compile_options(task_01_task_manager PRIVATE
        -Wall
        -Wextra
        -pedantic
    )
    target_compile_options(task_02_party PRIVATE
        -Wall
        -Wextra
        -pedantic
    )
endif()

