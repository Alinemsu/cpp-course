cmake_minimum_required(VERSION 3.16)
project(SparseMatrix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sparse_matrix main.cpp)
target_include_directories(sparse_matrix PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})