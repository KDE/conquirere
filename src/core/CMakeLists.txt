
set(core_LIB_SRCS
    library.cpp
    nepomukmodel.cpp
    publicationmodel.cpp
    publicationfiltermodel.cpp
    documentmodel.cpp
    notemodel.cpp
    bookmarkmodel.cpp
    ratingdelegate.cpp
    starrating.cpp
    htmldelegate.cpp
    searchresultmodel.cpp
)

nepomuk_add_ontology_classes(core_LIB_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig)
soprano_add_ontology(core_LIB_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk::Vocabulary" "trig")

kde4_add_library( core STATIC ${core_LIB_SRCS} )
 
install( TARGETS core  ${INSTALL_TARGETS_DEFAULT_ARGS} )


