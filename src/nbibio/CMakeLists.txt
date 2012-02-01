
set(nbibio_LIB_SRCS
    nbibexporter.cpp
    nbibexporterfile.cpp

    nbibimporter.cpp
    nbibimporterbibtex.cpp

    nbibsync.cpp
    zotero/synczoteronepomuk.cpp
    zotero/zoterodownload.cpp
    zotero/zoteroupload.cpp
    synckbibtexfile.cpp

    pipe/bibtexpipe.cpp
    pipe/bibtextoclipboardpipe.cpp
    pipe/bibtextonepomukpipe.cpp

    pipe/nepomukpipe.cpp
    pipe/nepomuktobibtexpipe.cpp

    pipe/kilelyxpipe.cpp
)

kde4_add_kcfg_files(nbibio_LIB_SRCS ../conquirere.kcfgc)
install(FILES ../conquirere.kcfg DESTINATION ${KCFG_INSTALL_DIR})


kde4_add_library( nbibio STATIC ${nbibio_LIB_SRCS} )

target_link_libraries(nbibio
# include static libs from conquirere
        ontology
)

install( TARGETS nbibio  ${INSTALL_TARGETS_DEFAULT_ARGS} )

