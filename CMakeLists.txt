cmake_minimum_required(VERSION 3.24)
project(exam)

set(CMAKE_CXX_STANDARD 20)

add_executable(exam_sync sync_instead_of_chief.cpp)
add_executable(exam main.cpp)
