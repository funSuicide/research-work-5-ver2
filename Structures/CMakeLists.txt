cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(Structures)
  
set(CMAKE_CXX_STANDARD 20)

add_library(Structures STATIC Structures.cpp)
target_include_directories(Structures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)