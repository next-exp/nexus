### --------------------------------------------------------
### File     : source/CMakeLists.txt
### Author   : Justo Martin-Albo
### Creation : 30 March 2019
### --------------------------------------------------------

set(SRC_DIRS  ${CMAKE_CURRENT_SOURCE_DIR}/actions
              ${CMAKE_CURRENT_SOURCE_DIR}/base
              ${CMAKE_CURRENT_SOURCE_DIR}/generators
              ${CMAKE_CURRENT_SOURCE_DIR}/geometries
              ${CMAKE_CURRENT_SOURCE_DIR}/materials
              ${CMAKE_CURRENT_SOURCE_DIR}/persistency
              ${CMAKE_CURRENT_SOURCE_DIR}/physics
              ${CMAKE_CURRENT_SOURCE_DIR}/physics_lists
              ${CMAKE_CURRENT_SOURCE_DIR}/sensdet
              ${CMAKE_CURRENT_SOURCE_DIR}/utils)

include_directories(${SRC_DIRS})

foreach(DIR ${SRC_DIRS})
  add_subdirectory(${DIR})
endforeach()
