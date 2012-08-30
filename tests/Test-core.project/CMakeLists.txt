

kde4_add_unit_test(TEST-core.project
  coreproject.cpp
)

target_link_libraries(TEST-core.project
# include static libs from conquirere
        ontology
        core
        nbibio
        #propertywidgets
        onlinestorage
        #mainui

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}
        #${KDE4_KPARTS_LIBS}
        #${KDE4_KHTML_LIBS}

        ${QT_QTTEST_LIBRARY}

#include nepomuk
        ${NEPOMUK_LIBRARIES}
        ${NEPOMUK_QUERY_LIBRARIES}
        nepomukcore
        nepomuksync

#include kbibtex
        ${KBIBTEX_LIBRARIES}

)
