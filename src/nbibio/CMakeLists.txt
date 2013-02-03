
add_subdirectory( provider )

set(nbibio_LIB_SRCS
    bibfileexporter.cpp
    bibfileimporter.cpp

    bibtex/bibtexvariant.cpp
    bibtex/bibtexexporter.cpp
    bibtex/bibteximporter.cpp

    pipe/clipboardpipe.cpp
    pipe/kilelyxpipe.cpp
    pipe/nepomuktovariantpipe.cpp
    pipe/varianttonepomukpipe.cpp

    nepomuksyncclient.cpp
    backgroundsync.cpp

    ui/bibtexexportdialog.cpp
    ui/bibteximportwizard.cpp
    ui/storagesyncwizard.cpp
    ui/itemdeletedialog.cpp
    ui/itemmergedialog.cpp
    ui/providersettings.cpp
)

kde4_add_ui_files( nbibio_LIB_SRCS
    ui/bibtexexportdialog.ui
    ui/bibteximportwizard.ui
    ui/storagesyncwizard.ui
    ui/itemdeletedialog.ui
    ui/itemmergedialog.ui
    ui/providersettings.ui
)

kde4_add_library(conquirere-nbibio STATIC ${nbibio_LIB_SRCS} )

target_link_libraries(conquirere-nbibio
    # include static libs from conquirere
    conquirere-ontology
    conquirere-core

    # storage provider
    conquirere-syncProvider
    conquirere-zoterosync

    # additional libs
    ${NEPOMUK_CORE_LIBRARY}
    ${NEPOMUK_WEBMINER_LIBS}

    ${KDE4_AKONADI_LIBS}
    ${KDE4_KABC_LIBS}
    akonadi-contact

    ${QT_QTNETWORK_LIBRARY}
)

#install( TARGETS nbibio ${INSTALL_TARGETS_DEFAULT_ARGS} )

