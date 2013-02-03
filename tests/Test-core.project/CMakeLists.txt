

kde4_add_unit_test(Test-core.project
  coreproject.cpp
)

target_link_libraries(Test-core.project
#include nepomuk
        ${NEPOMUK_CORE_LIBRARY}

# include static libs from conquirere
        conquirere-ontology
        conquirere-config
        conquirere-core
        conquirere-nbibio
        conquirere-propertywidgets
        conquirere-mainui

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}

        ${QT_QTTEST_LIBRARY}

#include kbibtex
        ${KBIBTEX_LIBRARIES}

)
