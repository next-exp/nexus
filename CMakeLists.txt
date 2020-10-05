### --------------------------------------------------------
### File     : CMakeLists.txt
### Author   : Justo Martin-Albo
### Creation : 30 March 2019
### --------------------------------------------------------

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(nexus)

##IF(COMMAND cmake_policy)
##  IF(${CMAKE_VERSION} VERSION_GREATER "3.8.2")
##    cmake_policy(SET CMP0074 NEW)
##  ENDIF(${CMAKE_VERSION} VERSION_GREATER "3.8.2")
##ENDIF(COMMAND cmake_policy)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Geant4 REQUIRED ui_all vis_all)
find_package(GSL REQUIRED)
find_package(HDF5 REQUIRED)
find_package(ROOT REQUIRED)

include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})

include_directories(${GSL_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

add_subdirectory(source)
