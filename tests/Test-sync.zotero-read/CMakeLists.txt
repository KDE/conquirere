

kde4_add_unit_test(Test-sync.zotero-read
  synczoteroread.cpp
)

target_link_libraries(Test-sync.zotero-read
# include static sync libs
        nbibio

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}

        ${QT_QTTEST_LIBRARY}
)
