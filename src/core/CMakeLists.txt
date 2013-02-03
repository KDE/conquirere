


set(core_LIB_SRCS
    projectsettings.cpp
    library.cpp
    librarymanager.cpp
    dirwatcher.cpp
    tagcloud.cpp

    delegates/ratingdelegate.cpp
    delegates/starrating.cpp
    delegates/htmldelegate.cpp

    models/nepomukmodel.cpp
    models/publicationmodel.cpp
    models/publicationfiltermodel.cpp
    models/documentmodel.cpp
    models/notemodel.cpp
    models/seriesmodel.cpp
    models/seriesfiltermodel.cpp
    models/referencemodel.cpp
    models/searchresultmodel.cpp
    models/eventmodel.cpp

    queryclients/queryclient.cpp
    queryclients/publicationquery.cpp
    queryclients/referencequery.cpp
    queryclients/documentquery.cpp
    queryclients/notequery.cpp
    queryclients/seriesquery.cpp
    queryclients/eventquery.cpp
)

kde4_add_library(conquirere-core STATIC ${core_LIB_SRCS} )
ADD_DEPENDENCIES(conquirere-core conquirere-config)
ADD_DEPENDENCIES(conquirere-core conquirere-ontology)

target_link_libraries(conquirere-core
        conquirere-config
        conquirere-ontology

        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}
        ${NEPOMUK_CORE_LIBRARY}
)
