### --------------------------------------------------------
### File     : source/tests/CMakeLists.txt
### Author   : Justo Martin-Albo
### Creation : 30 March 2019
### --------------------------------------------------------

### get_filename_component(<var> <FileName> <mode> [CACHE])
### Sets <var> to a component of <FileName>, where <mode> is
### NAME_WE = File name without directory or longest extension
get_filename_component(DIRNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
add_library(nexus_${DIRNAME} OBJECT ${SRCS})

############################################################
