## ---------------------------------------------------------
## nexus | CMakeLists.txt
##
## The NEXT Collaboration
## --------------------------------------------------------

project(nexus)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

find_package(Geant4 REQUIRED ui_all vis_all)
find_package(GSL REQUIRED)
find_package(HDF5 REQUIRED COMPONENTS CXX)
find_package(ROOT REQUIRED)

include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})

include_directories(${GSL_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

add_library(lib)
set_target_properties(lib PROPERTIES OUTPUT_NAME nexus)

add_executable(exe)
set_target_properties(exe PROPERTIES OUTPUT_NAME nexus)

set(SRC_DIRS  actions base generators geometries materials
              persistency physics physics_lists sensdet utils)

foreach(DIR ${SRC_DIRS})
  target_include_directories(lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/${DIR})
  target_include_directories(exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/${DIR})
  file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/${DIR}/*.cc)
  target_sources(lib PRIVATE ${SRC_FILES})
endforeach()

target_link_libraries(lib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} 
                          ${GSL_LIBRARIES} ${HDF5_LIBRARIES})

target_sources(exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/nexus.cc)
target_link_libraries(exe lib)
