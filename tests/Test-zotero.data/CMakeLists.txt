

kde4_add_unit_test(Test-zotero.data
  zoterodata.cpp
)

target_link_libraries(Test-zotero.data
# include static sync libs
        ontology
        core
        nbibio
        propertywidgets
        mainui

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}

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
)
