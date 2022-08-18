cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(yacp)

add_subdirectory(binja)

# Binary Ninja plugin ----------------------------------------------------------

set(PLUGIN_SOURCE
        Plugin.cpp CallgraphGenerator.cpp CallgraphGenerator.h)

add_library(yacp SHARED ${PLUGIN_SOURCE})
target_link_libraries(yacp binaryninjaapi)
target_compile_features(yacp PRIVATE cxx_std_17 c_std_99)
target_compile_definitions(yacp PRIVATE
        DEV_MODE=${DEV_MODE})

# Library targets linking against the Binary Ninja API need to be compiled with
# position-independent code on Linux.
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_compile_options(yacp PRIVATE "-fPIC")
endif()

# Support cmake --install
bn_install_plugin(yacp)
