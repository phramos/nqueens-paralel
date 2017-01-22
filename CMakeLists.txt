cmake_minimum_required(VERSION 3.6)
project(TP1_nqueens)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

#set(SOURCE_FILES ex_semaforo.c)
set(SOURCE_FILES main.c)
add_executable(PPD ${SOURCE_FILES})