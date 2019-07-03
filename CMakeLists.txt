### --------------------------------------------------------
### File     : CMakeLists.txt
### Author   : Justo Martin-Albo
### Creation : 30 March 2019
### --------------------------------------------------------

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(nexus)

cmake_policy(SET CMP0074 NEW)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(GATE REQUIRED)
find_package(Geant4 REQUIRED)
find_package(GSL REQUIRED)
find_package(HDF5 REQUIRED)
find_package(ROOT REQUIRED)

include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})

include_directories(${GATE_INCLUDE_DIRS})
include_directories(${GSL_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

add_subdirectory(source)
