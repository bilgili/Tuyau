# Copyright (c) 2017 Ahmet Bilgili ahmetbilgili@gmail.com

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Tuyau VERSION 1.0.0)
set(Tuyau_VERSION_ABI 100)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)

include(GitExternal)

set(TUYAU_DESCRIPTION "Paralel Filter Execution Framework")
set(TUYAU_MAINTAINER "Ahmet Bilgili <ahmetbilgili@gmail.com>")
set(CPACK_PACKAGE_LICENSE LGPL)
set(COMMON_PROJECT_DOMAIN org.doxygen)

include(Common)
common_find_package(Boost REQUIRED COMPONENTS system thread unit_test_framework)
common_find_package_post()

add_definitions(-DBOOST_PROGRAM_OPTIONS_DYN_LINK) # Fix for windows and shared boost.
add_subdirectory(tuyau)
add_subdirectory(tests)
add_subdirectory(doc)

include(CPackConfig)
set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md)
include(DoxygenRule)

