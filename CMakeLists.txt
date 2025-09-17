cmake_minimum_required(VERSION 4.0)
project(FramebufferDisplayLab C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(FramebufferDisplayLab main.c)
add_custom_command(TARGET FramebufferDisplayLab POST_BUILD
        COMMAND chmod +x $<TARGET_FILE:FramebufferDisplayLab>
)
