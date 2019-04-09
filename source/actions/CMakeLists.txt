### --------------------------------------------------------
### File     : CMakeLists.txt
### Author   : Justo Martin-Albo
### Creation : 30 March 2019
### --------------------------------------------------------

get_filename_component(DIRNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
add_library(nexus_${DIRNAME} OBJECT ${SRCS})
