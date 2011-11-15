cmake_minimum_required(VERSION 2.6)

project(conquirere)

# search packages used by KDE
find_package(KDE4 REQUIRED)
find_package (KdepimLibs REQUIRED)
find_package(Nepomuk REQUIRED)
include(NepomukAddOntologyClasses)
include (KDE4Defaults)
include (MacroLibrary)

# local cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules
)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT_STRICT_ITERATORS )
#add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories (${CMAKE_SOURCE_DIR}
                     ${CMAKE_BINARY_DIR}
                     ${KDE4_INCLUDES}
                     ${KDEPIMLIBS_INCLUDE_DIRS}
                     ${NEPOMUK_INCLUDE_DIR}
                     ${NEPOMUKEXTRAS_INCLUDE_DIR}
                     ${INCLUDE_INSTALL_DIR}/kbibtex
)

add_subdirectory( dms-copy )
add_subdirectory( src )
add_subdirectory( html )
add_subdirectory( xslt )
add_subdirectory( icons )

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
