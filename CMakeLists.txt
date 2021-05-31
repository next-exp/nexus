## ---------------------------------------------------------
## nexus | CMakeLists.txt
##
## The NEXT Collaboration
## --------------------------------------------------------

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(nexus)

find_package(Geant4 REQUIRED ui_all vis_all)
find_package(GSL REQUIRED)
find_package(HDF5 REQUIRED)
find_package(ROOT REQUIRED)

include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})

include_directories(${GSL_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

add_subdirectory(source)
