

project(zoterosync)

set(zoterosync_LIB_SRCS
    zoterosync.cpp
)

kde4_add_library( zoterosync STATIC ${zoterosync_LIB_SRCS} )

target_link_libraries(zoterosync
                      syncProvider
                      
                      ${QJSON_LIBRARIES} # added because the macro seems to exist with upper and lowercase depending on the distribution
                      ${qjson_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KDECORE_LIBS} )


install( TARGETS zoterosync ${INSTALL_TARGETS_DEFAULT_ARGS} )
KDE4_INSTALL_ICONS( ${DATA_INSTALL_DIR}/conquirere/icons )

install(FILES zoteromapping.ini DESTINATION ${DATA_INSTALL_DIR}/conquirere/provider )

