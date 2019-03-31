## --- FindLog4cpp.cmake ---
## Try to find Log4cpp library and headers.
## Once this is done, the following variables will be defined:
##  - Log4cpp_FOUND:        true if Log4cpp was found and false otherwise.
##  - Log4cpp_INCLUDE_DIRS: location of headers.
##  - Log4cpp_LIBRARIES:    libraries to link against.
##
## Variables used by this module which can be used to change the default
## behaviour, and hence need to be set before calling find_package:
##  - Log4cpp_ROOT_DIR: installation prefix for searching for Log4cpp.
##                      Set this if the module has problems finding your
##                      installation.
## If Log4cpp is installed in a non-standard way, e.g. a non GNU-style install
## of <prefix>/{lib,include}, then this module may fail to locate the headers
## and libraries as needed. In this case, the following cached variables can
## be editted to point to the correct locations.
##  - Log4cpp_INCLUDE_DIR: path to the Log4cpp include directory (cached)
##  - Log4cpp_LIBRARY:     path to the Log4cpp library (cached)
## You should not need to set these in the vast majority of cases.
##

find_path(Log4cpp_INCLUDE_DIR NAMES log4cpp HINTS ${Log4cpp_ROOT_DIR}/include)
mark_as_advanced(Log4cpp_INCLUDE_DIR)

find_library(Log4cpp_LIBRARY NAMES log4cpp HINTS ${Log4cpp_ROOT_DIR}/lib)
mark_as_advanced(Log4cpp_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Log4cpp
                                  FOUND_VAR     Log4cpp_FOUND
                                  REQUIRED_VARS Log4cpp_LIBRARY
                                                Log4cpp_INCLUDE_DIR)

if(Log4cpp_FOUND)
  set(Log4cpp_LIBRARIES ${Log4cpp_LIBRARY})
  set(Log4cpp_INCLUDE_DIRS ${Log4cpp_INCLUDE_DIR})
endif()
