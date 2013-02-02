
set(TESTDATADIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/testdatadir.h.in ${CMAKE_CURRENT_BINARY_DIR}/testdatadir.h @ONLY)

# Add Tests
add_subdirectory( Test-core.project )
add_subdirectory( Test-core.model )

#add_subdirectory( Test-nbibio.bibtexvariant )
#add_subdirectory( Test-nbibio.nepomuk2variant )
#add_subdirectory( Test-nbibio.variant2nepomuk )
#add_subdirectory( Test-nbibio.bibtex2bibtex )


#add_subdirectory( Test-zotero.simple )
#add_subdirectory( Test-zotero.data )


#add_subdirectory( Test-sync.zotero-read )
#add_subdirectory( Test-sync.zotero-write )
