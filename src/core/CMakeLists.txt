
set(core_LIB_SRCS
    library.cpp
    tagcloud.cpp

    delegates/ratingdelegate.cpp
    delegates/starrating.cpp
    delegates/htmldelegate.cpp

    models/nepomukmodel.cpp
    models/publicationmodel.cpp
    models/publicationfiltermodel.cpp
    models/documentmodel.cpp
    models/notemodel.cpp
    models/bookmarkmodel.cpp
    models/mailmodel.cpp
    models/seriesmodel.cpp
    models/referencemodel.cpp
    models/searchresultmodel.cpp

    queryclients/queryclient.cpp
    queryclients/publicationquery.cpp
    queryclients/referencequery.cpp
    queryclients/documentquery.cpp
    queryclients/notequery.cpp
    queryclients/bookmarkquery.cpp
    queryclients/mailquery.cpp
    queryclients/seriesquery.cpp
)

nepomuk_add_ontology_classes(core_LIB_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig)
soprano_add_ontology(core_LIB_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk::Vocabulary" "trig")

kde4_add_library( core STATIC ${core_LIB_SRCS} )
 
install( TARGETS core  ${INSTALL_TARGETS_DEFAULT_ARGS} )


