
add_subdirectory( nbibio )
add_subdirectory( core )
add_subdirectory( propertywidgets )
add_subdirectory( onlinestorage )
add_subdirectory( mainui )

set(conquirere_SRCS
    main.cpp
)

kde4_add_app_icon(conquirere_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/hi*-app-conquirere.png")
kde4_add_executable(conquirere ${conquirere_SRCS})

target_link_libraries(conquirere
# include static libs from conquirere
        ontology
        core
        nbibio
        propertywidgets
        onlinestorage
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
        nepomukquery
        #nepomukcore
        nepomukdatamanagement-copy

#include kbibtex
        kbibtexio
        kbibtexnetworking
        kbibtexgui
        kbibtexproc
)

install(TARGETS conquirere DESTINATION ${BIN_INSTALL_DIR})

install(FILES conquirere.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES conquirereui.rc    DESTINATION ${DATA_INSTALL_DIR}/conquirere)
