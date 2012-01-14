

kde4_add_unit_test(zoterowritetest
  zoterowritetest.cpp
)

target_link_libraries(zoterowritetest
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
        nepomukquery
        #nepomukcore
        nepomukdatamanagement-copy

#include kbibtex
        kbibtexio
        kbibtexnetworking
        kbibtexgui
        kbibtexproc

)
