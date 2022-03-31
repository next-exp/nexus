## ---------------------------------------------------------
## nexus | CMakeLists.txt
##
## The NEXT Collaboration
## --------------------------------------------------------

## Setup the project
cmake_minimum_required(VERSION 3.13)
project(nexus 
        DESCRIPTION "Geant4 simulation framework of the NEXT experiment" 
        LANGUAGES C CXX)

## 
set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

## Find dependencies
find_package(Geant4 REQUIRED ui_all vis_all)
find_package(GSL REQUIRED)
find_package(HDF5 REQUIRED)
find_package(ROOT 6.23 REQUIRED)

## Create library target and set its properties
add_library(lib SHARED)
set_target_properties(lib PROPERTIES OUTPUT_NAME ${PROJECT_NAME}
                                     EXPORT_NAME ${PROJECT_NAME})

## Create temporary folder to contain headers and it to the list of
## include directories of the library
include(GNUInstallDirs) # This defines GNU standard installation paths
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME})
target_include_directories(lib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>)

## Add external dependencies (libraries and includes) to library target 
target_include_directories(lib PRIVATE ${Geant4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                                       ${GSL_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})
target_link_libraries(lib PUBLIC ${Geant4_LIBRARIES} 
                          PRIVATE ${ROOT_LIBRARIES} ${GSL_LIBRARIES} ${HDF5_LIBRARIES})

## Define list with names of source folders
set(SOURCE_DIRS actions base generators geometries materials persistency 
                physics physics_lists sensdet utils)

## Loop through the directories
foreach(DIR ${SOURCE_DIRS})
  ## Create lists of *.h and *.cc files
  file(GLOB HDRS ${CMAKE_CURRENT_SOURCE_DIR}/source/${DIR}/*.h)
  file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/source/${DIR}/*.cc)
  # Add the list pf *.cc files to the sources of the library target
  target_sources(lib PRIVATE ${SRCS})
  ## Copy the *.h files to the 'include' folder created above
  file(COPY ${HDRS} DESTINATION ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME})
endforeach()

add_executable(exe)
set_target_properties(exe PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_sources(exe PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/nexus.cc)
target_link_libraries(exe PRIVATE lib)

add_executable(test)
set_target_properties(test PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-test)

file(GLOB TESTS ${CMAKE_CURRENT_SOURCE_DIR}/source/tests/*/*.cc)
target_sources(test PRIVATE ${TESTS} ${CMAKE_CURRENT_SOURCE_DIR}/source/nexus-test.cc)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/tests)
target_link_libraries(test PRIVATE lib)


install(TARGETS lib exe test
        EXPORT NexusTargets
        RUNTIME DESTINATION bin  
        LIBRARY DESTINATION lib)

install(EXPORT NexusTargets
        FILE NexusTargets.cmake
        DESTINATION lib/cmake/)

install(DIRECTORY ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME} DESTINATION include
        FILES_MATCHING PATTERN "*.h")

