# CMake-Script to find Nepomuk-WebMiner
# Once done this will define
#
#  NEPOMUK_WEBMINER_INCLUDE_DIRS - the kbibtex include directory
#  NEPOMUK_WEBMINER_LIBS - Link this to use the kbibtex library
#  NEPOMUK_WEBMINER_FOUND - True if found, otherwise false
#
# Copyright (c) 2012  Andreas Cord-Landwehr <cola@uni-paderborn.de>
# Copyright (c) 2013  Jörg Ehrichs <joerg.ehrichs@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if( NEPOMUK_WEBMINER_INCLUDE_DIRS AND NEPOMUK_WEBMINER_LIBS )
    # Already in cache, be silent
    set( NEPOMUK_WEBMINER_FIND_QUIETLY TRUE )
endif( NEPOMUK_WEBMINER_INCLUDE_DIRS AND NEPOMUK_WEBMINER_LIBS )

find_library(
    NEPOMUK_WEBMINER_LIB
    NAMES nepomuk-webminerlib
    HINTS ${KDE4_LIB_DIR} ${LIB_INSTALL_DIR}
)

find_path(
    NEPOMUK_WEBMINER_INCLUDE_DIRS
    NAMES nepomukwebminer_export.h
    HINTS ${KDE4_INCLUDE_DIR} ${INCLUDE_INSTALL_DIR}
)

# added because on my system the above statement can't find the includes in its usual subdirectory
if(NOT NEPOMUK_WEBMINER_INCLUDE_DIRS)
    find_path(
        NEPOMUK_WEBMINER_INCLUDE_DIRS
        NAMES nepomuk-webminer/nepomukwebminer_export.h
        HINTS ${KDE4_INCLUDE_DIR} ${INCLUDE_INSTALL_DIR}
    )
endif(NOT NEPOMUK_WEBMINER_INCLUDE_DIRS)

if (NEPOMUK_WEBMINER_INCLUDE_DIRS AND NEPOMUK_WEBMINER_LIB )
    # in cache already
    set(NEPOMUK_WEBMINER_FOUND TRUE)
    set(NEPOMUK_WEBMINER_INCLUDE_DIRS ${NEPOMUK_WEBMINER_INCLUDE_DIRS})
    set(NEPOMUK_WEBMINER_LIBS
        ${NEPOMUK_WEBMINER_LIB}
    )
endif(NEPOMUK_WEBMINER_INCLUDE_DIRS AND NEPOMUK_WEBMINER_LIB )

if( NEPOMUK_WEBMINER_FOUND )
    if( NOT NEPOMUK_WEBMINER_FIND_QUIETLY )
        message( STATUS "Found Nepomuk WebMiner libraries : ${NEPOMUK_WEBMINER_LIBS}" )
    endif( NOT NEPOMUK_WEBMINER_FIND_QUIETLY )
else( NEPOMUK_WEBMINER_FOUND )
    if( LibNepomukMetadataExtractor_FIND_REQUIRED )
        message( FATAL_ERROR "Could not find Nepomuk WebMiner libraries" )
    endif( LibNepomukMetadataExtractor_FIND_REQUIRED )
endif( NEPOMUK_WEBMINER_FOUND )


mark_as_advanced(
    NEPOMUK_WEBMINER_INCLUDE_DIRS
    NEPOMUK_INCLUDE_DIRS
)
