
add_definitions(-DQT_GUI_LIB)

kde4_add_unit_test(Test-sync.zotero-read
  synczoteroread.cpp
)

target_link_libraries(Test-sync.zotero-read
#include nepomuk
        ${NEPOMUK_CORE_LIBRARY}

# include static sync libs
        conquirere-ontology
        conquirere-core
        conquirere-nbibio

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
