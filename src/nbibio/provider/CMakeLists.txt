
add_subdirectory( zotero )
#add_subdirectory( mendeley )

set(syncprovider_LIB_SRCS
    onlinestorage.cpp
)

kde4_add_library(conquirere-syncProvider STATIC ${syncprovider_LIB_SRCS} )

#install( TARGETS conquirere-syncProvider ${INSTALL_TARGETS_DEFAULT_ARGS} )

