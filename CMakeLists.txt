cmake_minimum_required(VERSION 3.18)

project(box C)

set(CMAKE_C_STANDARD 11)

add_executable(box "main.c" "game.c" "buffer.c" "utils.c")
set_target_properties (box PROPERTIES COMMAND_LINE_ARGUMENTS "11 13")

add_executable(nyufile "nyufile.c" "nyuutils.c" "nyuinput.c" "nyudisk.c")