# - Try to find the kbibtex library
# Once done this will define
#
#  KBIBTEX_FOUND - system has the kbibtex library
#  KBIBTEX_INCLUDE_DIRS - the kbibtex include directory
#  KBIBTEX_LIBRARIES - Link this to use the kbibtex library
#
# Copyright (c) 2012, Jörg Ehrichs, <joerg.ehrichs@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if( KBIBTEX_INCLUDE_DIRS AND KBIBTEX_LIBRARIES )
    # Already in cache, be silent
    set( KBIBTEX_FIND_QUIETLY TRUE )
endif( KBIBTEX_INCLUDE_DIRS AND KBIBTEX_LIBRARIES )

find_library(
    KBIBTEXIO_LIBRARY
    NAMES kbibtexio
    HINTS ${KDE4_LIB_DIR} ${LIB_INSTALL_DIR}
)

find_library(
    KBIBTEXGUI_LIBRARY
    NAMES kbibtexgui
    HINTS ${KDE4_LIB_DIR} ${LIB_INSTALL_DIR}
)

find_library(
    KBIBTEXNETWORKING_LIBRARY
    NAMES kbibtexnetworking
    HINTS ${KDE4_LIB_DIR} ${LIB_INSTALL_DIR}
)

find_library(
    KBIBTEXPROC_LIBRARY
    NAMES kbibtexproc
    HINTS ${KDE4_LIB_DIR} ${LIB_INSTALL_DIR}
)

find_path(
    KBIBTEX_INCLUDE_DIRS
    NAMES kbibtexio_export.h
    HINTS ${KDE4_INCLUDE_DIR} ${INCLUDE_INSTALL_DIR} ${INCLUDE_INSTALL_DIR}
)

# added because on my system the above statement can't find the includes in its usual subdirectory
if(NOT KBIBTEX_INCLUDE_DIRS)
    find_path(
        KBIBTEX_INCLUDE_DIRS
        NAMES kbibtex/kbibtexio_export.h
        HINTS ${KDE4_INCLUDE_DIR} ${INCLUDE_INSTALL_DIR}
    )
endif(NOT KBIBTEX_INCLUDE_DIRS)

if (KBIBTEXIO_LIBRARY AND KBIBTEXGUI_LIBRARY AND KBIBTEXNETWORKING_LIBRARY AND KBIBTEXPROC_LIBRARY AND KBIBTEX_INCLUDE_DIRS)
    set(KBIBTEX_FOUND TRUE)
    set(KBIBTEX_LIBRARIES ${KBIBTEXIO_LIBRARY} ${KBIBTEXGUI_LIBRARY} ${KBIBTEXNETWORKING_LIBRARY} ${KBIBTEXPROC_LIBRARY})
endif(KBIBTEXIO_LIBRARY AND KBIBTEXGUI_LIBRARY AND KBIBTEXNETWORKING_LIBRARY AND KBIBTEXPROC_LIBRARY AND KBIBTEX_INCLUDE_DIRS)

# the header were only exported starting with 0.5
# as there is no other file to read/check for the versions string, we use this "workaround" at the moment
set(KBIBTEX_VERSION "0.4.0")
if(KBIBTEX_INCLUDE_DIRS)
  set(KBIBTEX_VERSION "0.5.0")
endif(KBIBTEX_INCLUDE_DIRS)

message(STATUS "Found KBibTeX" ${KBIBTEX_LIBRARIES} " Version (" ${KBIBTEX_VERSION} ")")

# set version variables
# todo find a way to do propper version check ...
set(KBIBTEX_VERSION "0.5.0")
STRING(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" KBIBTEX_VERSION_MAJOR "${KBIBTEX_VERSION}")
STRING(REGEX REPLACE "^[0-9]+\\.([0-9])+\\.[0-9]+.*" "\\1" KBIBTEX_VERSION_MINOR "${KBIBTEX_VERSION}")
STRING(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" KBIBTEX_VERSION_PATCH "${KBIBTEX_VERSION}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KBIBTEX DEFAULT_MSG KBIBTEX_INCLUDE_DIRS KBIBTEX_LIBRARIES KBIBTEX_VERSION)

set(KBIBTEX_VERSION ${KBIBTEX_VERSION} CACHE INTERNAL "The full version string")
set(KBIBTEX_VERSION_MAJOR ${KBIBTEX_VERSION_MAJOR} CACHE INTERNAL "The major version string")
set(KBIBTEX_VERSION_MINOR ${KBIBTEX_VERSION_MINOR} CACHE INTERNAL "The minot version string")
set(KBIBTEX_VERSION_PATCH ${KBIBTEX_VERSION_PATCH} CACHE INTERNAL "The path version string")

if( KBIBTEX_FOUND_FOUND )
    if( NOT KBIBTEX_FIND_QUIETLY )
        message( STATUS "Found KBibTex libraries: ${KBIBTEX_LIBRARIES}" )
    endif( NOT KBIBTEX_FIND_QUIETLY )
else( KBIBTEX_FOUND_FOUND )
    if( LibKBibTeX_FIND_REQUIRED )
        message( FATAL_ERROR "Could not find KBibTeX libraries" )
    endif( LibKBibTeX_FIND_REQUIRED )
endif( KBIBTEX_FOUND_FOUND )

mark_as_advanced(
    KBIBTEX_INCLUDE_DIRS
    KBIBTEX_LIBRARIES
)