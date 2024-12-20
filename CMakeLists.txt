cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(gost-provider)
  
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_compile_definitions(AUTHOR="@funSuicide") 

add_compile_options(-mavx2 -march=native -mavx512f -mavx512vl -mavx512bw -mavx512dq)
add_subdirectory(libprov)
include(libprov/cmake/provider.cmake)

setup_provider_openssl()

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/g_params.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/g_params.hpp
                   COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/libprov/perl/gen_param_LL.pl
                           ${CMAKE_CURRENT_SOURCE_DIR}/g_params.cpp
                           ${CMAKE_CURRENT_SOURCE_DIR}/g_params.hpp
                           ${CMAKE_CURRENT_SOURCE_DIR}/gost_params.dat
                   MAIN_DEPENDENCY gost_params.dat  
                   DEPENDS libprov/perl/gen_param_LL.pl)

set(gost_source customProvider.cpp g_params.cpp ./MagmaAVX2/MagmaAVX2.cpp ./Structures/Structures.cpp ./KuznechikAVX2/KuznechikAVX2.cpp 
./KuznechikAVX512/KuznechikAVX512.cpp ./MagmaAVX512/MagmaAVX512.cpp ./MagmaAVX512Reg/MagmaAVX512Reg.cpp
./KuznechikContextAVX2/KuznechikContextAVX2.cpp ./MagmaContextAVX2/MagmaContextAVX2.cpp ./MagmaContextAVX512/MagmaContextAVX512.cpp
./MagmaContextAVX512Reg/MagmaContextAVX512Reg.cpp ./KuznechikContextAVX512/KuznechikContextAVX512.cpp) 
set(gost_libs libprov)
build_provider(gost "${gost_source}" "${gost_libs}")


