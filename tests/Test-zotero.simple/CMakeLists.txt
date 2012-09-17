

kde4_add_unit_test(Test-zotero.simple
  zoterosimple.cpp
)

target_link_libraries(Test-zotero.simple
# include static sync libs
        nbibio

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}

        ${QT_QTTEST_LIBRARY}
)
