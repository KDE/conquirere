

kde4_add_unit_test(Test-zotero.data
  zoterodata.cpp
)

target_link_libraries(Test-zotero.data
#include nepomuk
        ${NEPOMUK_CORE_LIBRARY}

# include static sync libs
        conquirere-ontology
        conquirere-core
        conquirere-nbibio
        conquirere-propertywidgets
        conquirere-mainui

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}

        ${QT_QTTEST_LIBRARY}

#include akonadi stuff
        ${KDE4_AKONADI_LIBS}
        ${KDE4_KABC_LIBS}
        akonadi-contact

#include kbibtex
        ${KBIBTEX_LIBRARIES}
)
