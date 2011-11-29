
set(onlinestorage_LIB_SRCS
    readfromstorage.cpp
    writetostorage.cpp
    syncstorage.cpp
    zotero/readfromzotero.cpp
    zotero/writetozotero.cpp
    zotero/synczotero.cpp
)

kde4_add_library( onlinestorage STATIC ${onlinestorage_LIB_SRCS} )

target_link_libraries(onlinestorage
qjson
)
install( TARGETS onlinestorage  ${INSTALL_TARGETS_DEFAULT_ARGS} )


