
set(TESTDATADIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testdatadir.h.in ${CMAKE_CURRENT_BINARY_DIR}/testdatadir.h @ONLY)

# Add Tests
#add_subdirectory( bibtexnepomuktest )
#add_subdirectory( zoteroreadtest )
#add_subdirectory( zoterowritetest )


add_subdirectory( Test-core.project )

