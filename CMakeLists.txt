cmake_minimum_required(VERSION 3.25)
project(thread_pool)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(tpool ${SRC})