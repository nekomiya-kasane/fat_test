cmake_minimum_required(VERSION 3.18)

project(box C)

set(CMAKE_C_STANDARD 11)

add_executable(box "main.c" "game.c" "buffer.c" "utils.c")
set_target_properties (box PROPERTIES COMMAND_LINE_ARGUMENTS "11 13")

#set(OPENSSL_USE_STATIC_LIBS TRUE)
#find_package(OpenSSL REQUIRED)

#add_executable(nyufile "nyufile.c" "nyuutils.c" "nyuinput.c" "nyudisk.c" "nyurecover.c")

#set_target_properties(nyufile PROPERTIES 
#    VS_DEBUGGER_COMMAND_ARGUMENTS "disk")

#target_link_libraries(nyufile PUBLIC OpenSSL::Crypto m)

#include_directories(${OPENSSL_INCLUDE_DIR})