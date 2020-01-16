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
              ${CMAKE_CURRENT_SOURCE_DIR}/tests
              ${CMAKE_CURRENT_SOURCE_DIR}/utils)

include_directories(${SRC_DIRS})

foreach(DIR ${SRC_DIRS})
  add_subdirectory(${DIR})
endforeach()

############################################################

add_executable(nexus-test   nexus-test.cc
		                        $<TARGET_OBJECTS:nexus_actions>
                            $<TARGET_OBJECTS:nexus_base>
                            $<TARGET_OBJECTS:nexus_generators>
                            $<TARGET_OBJECTS:nexus_geometries>
                            $<TARGET_OBJECTS:nexus_materials>
                            $<TARGET_OBJECTS:nexus_persistency>
                            $<TARGET_OBJECTS:nexus_physics>
                            $<TARGET_OBJECTS:nexus_physics_lists>
                            $<TARGET_OBJECTS:nexus_sensdet>
                            $<TARGET_OBJECTS:nexus_tests>
                            $<TARGET_OBJECTS:nexus_utils>)

target_link_libraries(nexus-test ${ROOT_LIBRARIES}
                                 ${Geant4_LIBRARIES}
                                 ${GATE_LIBRARIES}
                                 ${HDF5_LIBRARIES}
                                 ${GSL_LIBRARIES})

############################################################

add_executable(nexus nexus.cc
		                 $<TARGET_OBJECTS:nexus_actions>
	                   $<TARGET_OBJECTS:nexus_base>
                     $<TARGET_OBJECTS:nexus_generators>
                     $<TARGET_OBJECTS:nexus_geometries>
                     $<TARGET_OBJECTS:nexus_materials>
                     $<TARGET_OBJECTS:nexus_persistency>
                     $<TARGET_OBJECTS:nexus_physics>
                     $<TARGET_OBJECTS:nexus_physics_lists>
                     $<TARGET_OBJECTS:nexus_sensdet>
                     $<TARGET_OBJECTS:nexus_utils>)

target_link_libraries(nexus ${ROOT_LIBRARIES}
                            ${Geant4_LIBRARIES}
                            ${GATE_LIBRARIES}
                            ${HDF5_LIBRARIES}
                            ${GSL_LIBRARIES})

############################################################

install(TARGETS nexus nexus-test RUNTIME DESTINATION bin)
