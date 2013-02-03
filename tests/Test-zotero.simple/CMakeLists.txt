

kde4_add_unit_test(Test-zotero.simple
  zoterosimple.cpp
)

target_link_libraries(Test-zotero.simple
#include nepomuk
        ${NEPOMUK_CORE_LIBRARY}

# include static sync libs
        conquirere-core
        conquirere-nbibio

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}

        ${QT_QTTEST_LIBRARY}

)
