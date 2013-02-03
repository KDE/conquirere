

kde4_add_unit_test(Test-core.model
  coremodel.cpp
)

target_link_libraries(Test-core.model
#include nepomuk
        ${NEPOMUK_CORE_LIBRARY}

# include static libs from conquirere
        conquirere-core

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        #${KDE4_KIO_LIBS}

        ${QT_QTTEST_LIBRARY}

#include kbibtex
        #${KBIBTEX_LIBRARIES}

)
