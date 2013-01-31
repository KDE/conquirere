

project(mendeleysync)

set(mendeley_LIB_SRCS
    readfrommendeley.cpp
    mendeleyclient.cpp
)

kde4_add_library( mendeleysync STATIC ${mendeley_LIB_SRCS} )

target_link_libraries(mendeleysync
                      qoauth
                      ${QJSON_LIBRARIES} # added because the macro seems to exist with upper and lowercase depending on the distribution
                      ${qjson_LIBRARIES}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KDECORE_LIBS} )


install( TARGETS mendeleysync  ${INSTALL_TARGETS_DEFAULT_ARGS} )
KDE4_INSTALL_ICONS( ${DATA_INSTALL_DIR}/publicationdata-sync/icons )

