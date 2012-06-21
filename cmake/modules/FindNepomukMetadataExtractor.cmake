# CMake-Script to find Nepomuk-Metadata-Extractor
# Once done this will define
#
#  NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS - the kbibtex include directory
#  NEPOMUK_METADATA_EXTRACTOR_LIBS - Link this to use the kbibtex library
#  NEPOMUK_METADATA_EXTRACTOR_FOUND - True if found, otherwise false
#
# Copyright (c) 2012  Andreas Cord-Landwehr <cola@uni-paderborn.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if( NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS AND NEPOMUK_METADATA_EXTRACTOR_LIBS )
    # Already in cache, be silent
    set( NEPOMUK_METADATA_EXTRACTOR_FIND_QUIETLY TRUE )
endif( NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS AND NEPOMUK_METADATA_EXTRACTOR_LIBS )

find_library(
    NEPOMUK_METADATA_EXTRACTOR_LIB
    NAMES libnepomukmetadataextractor
    HINTS ${KDE4_LIB_DIR} ${LIB_INSTALL_DIR}
)

find_path(
    NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS
    NAMES nepomukmetadataextractor_export.h
    HINTS ${KDE4_INCLUDE_DIR} ${INCLUDE_INSTALL_DIR}
)

# added because on my system the above statement can't find the includes in its usual subdirectory
if(NOT NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS)
    find_path(
        NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS
        NAMES nepomukmetadataextractor/nepomukmetadataextractor_export.h
        HINTS ${KDE4_INCLUDE_DIR} ${INCLUDE_INSTALL_DIR}
    )
endif(NOT NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS)

if (NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS AND NEPOMUK_METADATA_EXTRACTOR_LIB )
    # in cache already
    set(NEPOMUK_METADATA_EXTRACTOR_FOUND TRUE)
    set(NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS ${NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS})
    set(NEPOMUK_METADATA_EXTRACTOR_LIBS
        ${NEPOMUK_METADATA_EXTRACTOR_LIB}
    )
endif(NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS AND NEPOMUK_METADATA_EXTRACTOR_LIB )

if( NEPOMUK_METADATA_EXTRACTOR_FOUND )
    if( NOT NEPOMUK_METADATA_EXTRACTOR_FIND_QUIETLY )
        message( STATUS "Found Nepomuk Metadata Extractor libraries : ${NEPOMUK_METADATA_EXTRACTOR_LIBS}" )
    endif( NOT NEPOMUK_METADATA_EXTRACTOR_FIND_QUIETLY )
else( NEPOMUK_METADATA_EXTRACTOR_FOUND )
    if( LibNepomukMetadataExtractor_FIND_REQUIRED )
        message( FATAL_ERROR "Could not find Nepomuk Metadata Extractor libraries" )
    endif( LibNepomukMetadataExtractor_FIND_REQUIRED )
endif( NEPOMUK_METADATA_EXTRACTOR_FOUND )


mark_as_advanced(
    NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS
    NEPOMUK_INCLUDE_DIRS
)
