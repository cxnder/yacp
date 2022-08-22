cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(yacp)

add_subdirectory(binja)

set(CMAKE_REQUIRED_LINK_OPTIONS "-arch arm64;-arch x86_64")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE INTERNAL "" FORCE)

# Binary Ninja plugin ----------------------------------------------------------

set(PLUGIN_SOURCE
        Plugin.cpp CallgraphGenerator.cpp CallgraphGenerator.h)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)


add_library(yacp SHARED ${PLUGIN_SOURCE})
target_link_libraries(yacp binaryninjaapi binaryninjaui Qt6::Core Qt6::Gui Qt6::Widgets)
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
