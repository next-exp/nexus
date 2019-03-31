## --- FindGATE.cmake ---
## Try to find GATE library and headers.
## Once this is done, the following variables will be defined:
##  - GATE_FOUND:        true if GATE was found and false otherwise.
##  - GATE_INCLUDE_DIRS: location of headers.
##  - GATE_LIBRARIES:    libraries to link against.
##
## Variables used by this module which can be used to change the default
## behaviour, and hence need to be set before calling find_package:
##  - GATE_ROOT_DIR: installation prefix for searching for GATE.
##                   Set this if the module has problems finding your
##                   installation.
## If GATE is installed in a non-standard way, e.g. a non GNU-style install
## of <prefix>/{lib,include}, then this module may fail to locate the headers
## and libraries as needed. In this case, the following cached variables can
## be editted to point to the correct locations.
##  - GATE_INCLUDE_DIR: path to the GATE include directory (cached)
##  - GATE_LIBRARY:     path to the GATE library (cached)
## You should not need to set these in the vast majority of cases.
##

find_path(GATE_INCLUDE_DIR NAMES GATE HINTS ${GATE_ROOT_DIR}/include)
mark_as_advanced(GATE_INCLUDE_DIR)

find_library(GATE_LIBRARY NAMES GATE HINTS ${GATE_ROOT_DIR}/lib)
mark_as_advanced(GATE_LIBRARY)

find_library(GATEIO_LIBRARY NAMES GATEIO HINTS ${GATE_ROOT_DIR}/lib)
mark_as_advanced(GATEIO_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GATE
    FOUND_VAR GATE_FOUND
    REQUIRED_VARS GATE_LIBRARY GATEIO_LIBRARY GATE_INCLUDE_DIR
    FAIL_MESSAGE "Set GATE_ROOT_DIR to the directory that contains your GATE installation.")

if(GATE_FOUND)
  set(GATE_LIBRARIES ${GATE_LIBRARY} ${GATEIO_LIBRARY})
  set(GATE_INCLUDE_DIRS ${GATE_INCLUDE_DIR})
endif()
