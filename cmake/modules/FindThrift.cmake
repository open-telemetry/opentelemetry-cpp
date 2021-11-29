# This module defines Thrift_LIBRARIES, libraries to link Thrift_INCLUDE_DIR,
# Thrift_FOUND, If false, do not try to use it.

# prefer the Thrift version supplied in Thrift_HOME (cmake -DThrift_HOME then
# environment)
find_path(
  Thrift_INCLUDE_DIR
  NAMES thrift/Thrift.h
  HINTS ${Thrift_HOME} ENV Thrift_HOME /usr/local /opt/local
  PATH_SUFFIXES include)

# prefer the Thrift version supplied in Thrift_HOME
find_library(
  Thrift_LIBRARIES
  NAMES thrift libthrift
  HINTS ${Thrift_HOME} ENV Thrift_HOME /usr/local /opt/local
  PATH_SUFFIXES lib lib64)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Thrift DEFAULT_MSG Thrift_LIBRARIES
                                  Thrift_INCLUDE_DIR)
mark_as_advanced(Thrift_LIBRARIES Thrift_INCLUDE_DIR)


if(Thrift_FOUND AND NOT (TARGET thrift::thrift))
  add_library(thrift::thrift UNKNOWN IMPORTED)
  set_target_properties(
    thrift::thrift
    PROPERTIES IMPORTED_LOCATION ${Thrift_LIBRARIES}
	       INTERFACE_INCLUDE_DIRECTORIES ${Thrift_INCLUDE_DIR})
endif()
