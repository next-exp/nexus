## ---------------------------------------------------------
## nexus | CMakeLists.txt
##
## The NEXT Collaboration
## --------------------------------------------------------

cmake_minimum_required(VERSION 3.12)

project(nexus 
        DESCRIPTION "Geant4 simulation framework of the NEXT experiment" 
        LANGUAGES C CXX)

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Find dependencies
find_package(Geant4 REQUIRED ui_all vis_all)
find_package(GSL REQUIRED)
find_package(HDF5 REQUIRED)

# Define list with names of source folders
set(SOURCE_DIRS actions base generators geometries materials
                persistency physics physics_lists sensdet utils)

# Create temporary folder to contain headers
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME})


add_library(lib SHARED)
set_target_properties(lib PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

foreach(DIR ${SOURCE_DIRS})
  # Create lists of *.h and *.cc files
  file(GLOB HDRS ${CMAKE_SOURCE_DIR}/source/${DIR}/*.h)
  file(GLOB SRCS ${CMAKE_SOURCE_DIR}/source/${DIR}/*.cc)
  # Copy header files to the 'include' binary folder
  file(COPY ${HDRS} DESTINATION ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME})

  target_include_directories(lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>)

  target_sources(lib PRIVATE ${SRCS})
endforeach()

target_include_directories(lib PRIVATE ${Geant4_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
target_link_libraries(lib PUBLIC 
                      ${Geant4_LIBRARIES} PRIVATE
                      ${GSL_LIBRARIES} ${HDF5_LIBRARIES})

add_executable(exe)
set_target_properties(exe PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_sources(exe PRIVATE ${CMAKE_SOURCE_DIR}/source/nexus.cc)
target_link_libraries(exe PRIVATE lib)

add_executable(test)
set_target_properties(test PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-test)

file(GLOB TESTS ${CMAKE_SOURCE_DIR}/source/tests/*/*.cc)
target_sources(test PRIVATE ${TESTS} ${CMAKE_SOURCE_DIR}/source/nexus-test.cc)
target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/source/tests)
target_link_libraries(test PRIVATE lib)


install(TARGETS lib exe test
        RUNTIME DESTINATION bin  
        LIBRARY DESTINATION lib)

install(DIRECTORY ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME} DESTINATION include
        FILES_MATCHING PATTERN "*.h")

