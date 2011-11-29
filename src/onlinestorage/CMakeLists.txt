
set(onlinestorage_LIB_SRCS
    readfromstorage.cpp
    writetostorage.cpp
    syncstorage.cpp
    syncstorageui.cpp
    zotero/readfromzotero.cpp
    zotero/writetozotero.cpp
    zotero/synczotero.cpp
)

kde4_add_ui_files( onlinestorage_LIB_SRCS
    syncstorageui.ui
)

kde4_add_library( onlinestorage STATIC ${onlinestorage_LIB_SRCS} )

target_link_libraries(onlinestorage
                      qjson
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KDECORE_LIBS} )

install( TARGETS onlinestorage  ${INSTALL_TARGETS_DEFAULT_ARGS} )


