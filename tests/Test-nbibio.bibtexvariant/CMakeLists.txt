

kde4_add_unit_test(Test-nbibio.bibtexvariant
  bibtexvarianttest.cpp
)

target_link_libraries(Test-nbibio.bibtexvariant

# include static libs from conquirere
        ontology
        core
        nbibio
        propertywidgets
        onlinestorage
        mainui

# include general kde libs
        ${KDE4_KDECORE_LIBS}
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

#include kbibtex
        ${KBIBTEX_LIBRARIES}

)
