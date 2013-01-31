


add_subdirectory( config )
add_subdirectory( core )
add_subdirectory( nbibio )
add_subdirectory( mainui )
add_subdirectory( propertywidgets )

set(conquirere_SRCS
    main.cpp
)

kde4_add_app_icon(conquirere_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/hi*-app-conquirere.png")
kde4_add_executable(conquirere ${conquirere_SRCS})

ADD_DEPENDENCIES(conquirere version)
ADD_DEPENDENCIES(conquirere config)

target_link_libraries(conquirere
# include static libs from conquirere
        ontology
        config
        core
        nbibio
        propertywidgets
        mainui

# include general kde libs
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${KDE4_KIO_LIBS}
        ${KDE4_KPARTS_LIBS}
        ${KDE4_KHTML_LIBS}

#include akonadi stuff
        ${KDE4_AKONADI_LIBS}
        ${KDE4_KABC_LIBS}
        akonadi-contact

#include nepomuk
        ${NEPOMUK_LIBRARIES}
        ${NEPOMUK_QUERY_LIBRARIES}
        nepomukcore

#include kbibtex
        ${KBIBTEX_LIBRARIES}

        ${NEPOMUK_WEBMINER_LIBS}
)

install(TARGETS conquirere DESTINATION ${BIN_INSTALL_DIR})

install(FILES conquirere.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES conquirereui.rc    DESTINATION ${DATA_INSTALL_DIR}/conquirere)
