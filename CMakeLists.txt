cmake_minimum_required(VERSION 3.4)
project(cmd)
add_executable(main front.c msg_operation.h msg_operation.c)
add_executable(back back.c msg_operation.h msg_operation.c)