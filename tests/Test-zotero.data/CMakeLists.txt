

kde4_add_unit_test(Test-zotero.data
  zoterodata.cpp
)

target_link_libraries(Test-zotero.data
# include static sync libs
        nbibio

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}

        ${QT_QTTEST_LIBRARY}
)
