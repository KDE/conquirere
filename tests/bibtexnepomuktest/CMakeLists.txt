

kde4_add_unit_test(bibtexnepomuktest
  TESTNAME core-bibtexnepomuktest
  bibtexnepomuktest.cpp
)

target_link_libraries(bibtexnepomuktest
# include static libs from conquirere
        ontology
        core
        nbibio
        propertywidgets
        onlinestorage
        mainui

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}
        ${KDE4_KPARTS_LIBS}
        ${KDE4_KHTML_LIBS}

        ${QT_QTTEST_LIBRARY}

#include akonadi stuff
        ${KDE4_AKONADI_LIBS}
        ${KDE4_KABC_LIBS}
        akonadi-contact

#include nepomuk
        ${NEPOMUK_LIBRARIES}
        ${NEPOMUK_QUERY_LIBRARIES}
        nepomukcore
        nepomuksync

#include kbibtex
        ${KBIBTEX_LIBRARIES}
        #kbibtexio
        #kbibtexnetworking
        #kbibtexgui
        #kbibtexproc

)
