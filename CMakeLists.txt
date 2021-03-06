cmake_minimum_required(VERSION 2.6)

project(conquirere)

# local cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake/modules
)

# CTest setup
# ===============================================================================================
ENABLE_TESTING()
include(CTest)
include(CTestConfig.cmake)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake COPYONLY)

# search required packages
# ===============================================================================================
set(KDE_MIN_VERSION "4.9.90")  # for the < 4.2 macro
find_package(KDE4 4.9.0 REQUIRED)
find_package(KdepimLibs REQUIRED)

set(SDO_MIN_VERSION "0.10.0")
find_package(SharedDesktopOntologies)
find_package(Soprano)
find_package(NepomukCore REQUIRED)

find_package(QJSON REQUIRED)
set(KBIBTEX_MIN_VERSION "0.5.0")
find_package(KBibTeX REQUIRED)
set(NEPOMUK_WEBMINER_MIN_VERSION "0.4.0")
find_package(NepomukWebMiner REQUIRED)

include(NepomukAddOntologyClasses)
include(KDE4Defaults)
include(MacroLibrary)

macro_log_feature(SHAREDDESKTOPONTOLOGIES_FOUND "Shared desktop ontologies" "Desktop ontologies" "http://oscaf.sourceforge.net" TRUE ${SDO_MIN_VERSION} "Ontologies necessary for the Nepomuk semantic desktop.")
macro_log_feature(NEPOMUK_WEBMINER_FOUND "Nepomuk-WebMiner" "Internet lookup/search" "https://projects.kde.org/projects/extragear/base/nepomuk-webminer" TRUE ${NEPOMUK_WEBMINER_MIN_VERSION} "WebMiner is necessary for the publication/metadata search.")
macro_log_feature(KBIBTEX_FOUND "KBibTeX" ".bib file handling" "http://home.gna.org/kbibtex/" TRUE ${KBIBTEX_MIN_VERSION} "KBibTeX is necessary for the im/export of .bib files.")



# Some definitions
# ===============================================================================================
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DQT_STRICT_ITERATORS )
#add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions(-DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DDISABLE_NEPOMUK_LEGACY)


# Set a version that will be used if we're not building from git
# ===============================================================================================
set(VERSION 0.1pre)

if(EXISTS "${conquirere_SOURCE_DIR}/.git")
  find_program(GIT_EXECUTABLE NAMES git)
  if(GIT_EXECUTABLE)
    message(STATUS "Found git: ${GIT_EXECUTABLE}")
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --abbrev=4 HEAD
                    WORKING_DIRECTORY ${conquirere_SOURCE_DIR}
                    OUTPUT_VARIABLE conquirere_git_revision)

    string(REGEX REPLACE "\n" "" conquirere_git_revision "${conquirere_git_revision}")
    string(REGEX REPLACE "^v(.*)" "\\\\1" VERSION ${conquirere_git_revision})
  endif()
endif()

configure_file(${conquirere_SOURCE_DIR}/version.h.in ${conquirere_BINARY_DIR}/version.h @ONLY)


# Add all the parts that make up conquirere
# ===============================================================================================
include_directories (
                     ${KDE4_INCLUDES}
                     ${SOPRANO_INCLUDE_DIR}
                     ${NEPOMUK_CORE_INCLUDE_DIR}
                     ${KBIBTEX_INCLUDE_DIRS}
                     ${NEPOMUK_WEBMINER_INCLUDE_DIRS}
                     ${NEPOMUK_WEBMINER_INCLUDE_DIRS}/nepomuk-webminer/

                     ${CMAKE_SOURCE_DIR}
                     ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/src/sro
                     ${CMAKE_SOURCE_DIR}/src/mainui
                     ${CMAKE_BINARY_DIR}
                     ${CMAKE_BINARY_DIR}/ontology
                     ${CMAKE_BINARY_DIR}/src
                     ${CMAKE_BINARY_DIR}/src/mainui
                     ${CMAKE_BINARY_DIR}/src/nbibio/ui
)


add_subdirectory( ontology )
add_subdirectory( src )
add_subdirectory( html )
add_subdirectory( icons )
add_subdirectory( icons/app )
add_subdirectory( icons/actions )
add_subdirectory( tests )

# add a target to generate API documentation with Doxygen
# ===============================================================================================
find_package(Doxygen)
if(DOXYGEN_FOUND)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
                         ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	                 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	                 COMMENT "Generating API documentation with Doxygen" VERBATIM
                      )
endif(DOXYGEN_FOUND)

macro_display_feature_log()
