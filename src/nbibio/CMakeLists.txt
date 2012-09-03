
set(nbibio_LIB_SRCS
    bibfileexporter.cpp
    bibfileimporter.cpp

    bibtex/bibtexvariant.cpp
    bibtex/bibtexexporter.cpp
    bibtex/bibteximporter.cpp
    bibtex/synckbibtexfile.cpp

    nbibsync.cpp
    zotero/synczoteronepomuk.cpp
    zotero/zoterodownload.cpp
    zotero/zoteroupload.cpp

    pipe/clipboardpipe.cpp
    pipe/kilelyxpipe.cpp
    pipe/nepomuktovariantpipe.cpp
    pipe/varianttonepomukpipe.cpp
)

kde4_add_kcfg_files(nbibio_LIB_SRCS ../conquirere.kcfgc)
install(FILES ../conquirere.kcfg DESTINATION ${KCFG_INSTALL_DIR})


kde4_add_library( nbibio STATIC ${nbibio_LIB_SRCS} )

target_link_libraries(nbibio
    # include static libs from conquirere
    ontology

    # additional libs
    ${NEPOMUK_METADATA_EXTRACTOR_LIBS}
)

install( TARGETS nbibio  ${INSTALL_TARGETS_DEFAULT_ARGS} )

