# - Try to find the kbibtex library
# Once done this will define
#
#  KBIBTEX_FOUND - system has the kbibtex library
#  KBIBTEX_INCLUDE_DIR - the kbibtex include directory
#  KBIBTEX_LIBRARIES - Link this to use the kbibtex library
#
# Copyright (c) 2012, Jörg Ehrichs, <joerg.ehrichs@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (KBIBTEX_LIBRARIES AND KBIBTEX_INCLUDE_DIR)
  # in cache already
  set(KBIBTEX_FOUND TRUE)
else (KBIBTEX_LIBRARIES AND KBIBTEX_INCLUDE_DIR)

  find_path(KBIBTEX_INCLUDE_DIR kbibtexio_export.h
    ${GNUWIN32_DIR}/include
    PATH_SUFFIXES kbibtex
  )

  find_library(KBIBTEX_LIBRARIES NAMES kbibtexgui
    PATHS
    ${GNUWIN32_DIR}/lib
  )
  find_library(KBIBTEX_LIBRARIES2 NAMES kbibtexio
    PATHS
    ${GNUWIN32_DIR}/lib
  )
  LIST(APPEND KBIBTEX_LIBRARIES ${KBIBTEX_LIBRARIES2})
  
  find_library(KBIBTEX_LIBRARIES3 NAMES kbibtexnetworking
    PATHS
    ${GNUWIN32_DIR}/lib
  )
  LIST(APPEND KBIBTEX_LIBRARIES ${KBIBTEX_LIBRARIES3})
  
  find_library(KBIBTEX_LIBRARIES4 NAMES kbibtexproc
    PATHS
    ${GNUWIN32_DIR}/lib
  )
  LIST(APPEND KBIBTEX_LIBRARIES ${KBIBTEX_LIBRARIES4})
  
  MESSAGE(STATUS "librarys" ${KBIBTEX_LIBRARIES})
  
    # set version variables
    # todo find a way to do propper version check ...
  set(KBIBTEX_VERSION "0.5.0")
  STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" KBIBTEX_VERSION_MAJOR "${KBIBTEX_VERSION}")
  STRING(REGEX REPLACE "^[0-9]+\\.([0-9])+\\.[0-9]+.*" "\\1" KBIBTEX_VERSION_MINOR "${KBIBTEX_VERSION}")
  STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" KBIBTEX_VERSION_PATCH "${KBIBTEX_VERSION}")

  include(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(KBIBTEX DEFAULT_MSG KBIBTEX_INCLUDE_DIR KBIBTEX_LIBRARIES KBIBTEX_VERSION)
  # ensure that they are cached
  set(KBIBTEX_INCLUDE_DIR ${KBIBTEX_INCLUDE_DIR} CACHE INTERNAL "The kbibtex include path")
  set(KBIBTEX_LIBRARIES ${KBIBTEX_LIBRARIES} CACHE INTERNAL "The libraries needed to use kbibtex")
  
  set(KBIBTEX_VERSION ${KBIBTEX_VERSION} CACHE INTERNAL "The full version string")
  set(KBIBTEX_VERSION_MAJOR ${KBIBTEX_VERSION_MAJOR} CACHE INTERNAL "The major version string")
  set(KBIBTEX_VERSION_MINOR ${KBIBTEX_VERSION_MINOR} CACHE INTERNAL "The minot version string")
  set(KBIBTEX_VERSION_PATCH ${KBIBTEX_VERSION_PATCH} CACHE INTERNAL "The path version string")
  

endif (KBIBTEX_LIBRARIES AND KBIBTEX_INCLUDE_DIR)

