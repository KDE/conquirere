
add_subdirectory( provider/zotero )
#add_subdirectory( provider/mendeley )

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
    ui/synczoterodialog.cpp
    ui/storagesyncwizard.cpp
    ui/itemdeletedialog.cpp
    ui/itemmergedialog.cpp
)

kde4_add_ui_files( nbibio_LIB_SRCS
    ui/bibtexexportdialog.ui
    ui/bibteximportwizard.ui
    ui/storagesyncwizard.ui
    ui/itemdeletedialog.ui
    ui/itemmergedialog.ui
)

kde4_add_library( nbibio STATIC ${nbibio_LIB_SRCS} )

target_link_libraries(nbibio
    # include static libs from conquirere
    ontology

    # additional libs
    ${NEPOMUK_METADATA_EXTRACTOR_LIBS}
)

install( TARGETS nbibio  ${INSTALL_TARGETS_DEFAULT_ARGS} )

