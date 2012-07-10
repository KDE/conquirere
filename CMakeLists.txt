cmake_minimum_required(VERSION 2.6)

project(conquirere)

# local cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules
)

set(KDE_MIN_VERSION "4.5.80")  # for the < 4.2 macro
find_package(KDE4 REQUIRED)
find_package(KdepimLibs REQUIRED)

set(SDO_MIN_VERSION "0.8.1")
find_package(SharedDesktopOntologies)
find_package(Soprano)
find_package(NepomukCore REQUIRED)

find_package(QJSON REQUIRED)
#set(KBIBTEX_MIN_VERSION "0.6.0")
find_package(KBibTeX REQUIRED)
find_package(NepomukMetadataExtractor REQUIRED)

include (NepomukAddOntologyClasses)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT_STRICT_ITERATORS )
#add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions(-DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories (
                     ${KDE4_INCLUDES}
                     ${KDEPIMLIBS_INCLUDE_DIRS}
                     ${NEPOMUK_INCLUDE_DIR}
                     ${NEPOMUKEXTRAS_INCLUDE_DIR}
                     ${KBIBTEX_INCLUDE_DIRS}
                     ${NEPOMUK_METADATA_EXTRACTOR_INCLUDE_DIRS}

                     ${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/sro
                     ${CMAKE_SOURCE_DIR}/src/mainui
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_BINARY_DIR}/src
                     ${CMAKE_BINARY_DIR}/src/mainui
                     ${CMAKE_BINARY_DIR}/nbib
)

# create versions string
FILE(WRITE ${CMAKE_BINARY_DIR}/version.h.in
    "\#define VERSION \"@VERSION@\"\n"
)

FILE(WRITE ${CMAKE_BINARY_DIR}/version.cmake
    "execute_process(COMMAND git describe --abbrev=4 HEAD
                   COMMAND sed -e "s/-/./g"
                   OUTPUT_VARIABLE VERSION
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
    # remove leading "v"
    string(REGEX REPLACE "^v(.*)" "\\1" VERSION ${VERSION})

    CONFIGURE_FILE(\${SRC} \${DST} @ONLY)
")

ADD_CUSTOM_TARGET(
    version
    ${CMAKE_COMMAND} -D SRC=${CMAKE_BINARY_DIR}/version.h.in
                     -D DST=${CMAKE_BINARY_DIR}/version.h
                     -P ${CMAKE_BINARY_DIR}/version.cmake
)

add_subdirectory( nbib )
add_subdirectory( src )
add_subdirectory( html )
add_subdirectory( xslt )
add_subdirectory( icons )
add_subdirectory( icons/app )
add_subdirectory( icons/actions )
add_subdirectory( tests )

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
                         ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	                 COMMENT "Generating API documentation with Doxygen" VERBATIM
                      )
endif(DOXYGEN_FOUND)
