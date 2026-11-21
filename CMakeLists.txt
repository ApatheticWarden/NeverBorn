cmake_minimum_required(VERSION 3.16)
project(NeverBorn CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/core/input.cpp
    src/render/graphics.cpp
    src/render/model_manager.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    src/core
    src/game
    src/render
)

# Флаги под PSP / VFPU (при необходимости подстрой под свои)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -O2
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspgum
    pspvfpu
    pspgu
    pspge
    pspdisplay
    pspctrl
    psppower
    m
)

create_pbp_file(
    TARGET ${PROJECT_NAME}
    TITLE "NeverBorn"
)