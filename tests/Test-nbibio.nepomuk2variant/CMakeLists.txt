

kde4_add_unit_test(Test-nbibio.nepomuk2variant
  nepomukvariant.cpp
)

target_link_libraries(Test-nbibio.nepomuk2variant
#include nepomuk
        ${NEPOMUK_CORE_LIBRARY}

# include static libs from conquirere
        conquirere-ontology
        conquirere-core
        conquirere-nbibio
        conquirere-propertywidgets
        conquirere-mainui

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}
        #${KDE4_KPARTS_LIBS}
        #${KDE4_KHTML_LIBS}

        ${QT_QTTEST_LIBRARY}

#include akonadi stuff
        ${KDE4_AKONADI_LIBS}
        ${KDE4_KABC_LIBS}
        akonadi-contact

#include kbibtex
        ${KBIBTEX_LIBRARIES}

)
