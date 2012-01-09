
set(nbibio_LIB_SRCS
    nbibexporter.cpp
    nbibexporterfile.cpp

    nbibimporter.cpp
    nbibimporterbibtex.cpp

    nbibsync.cpp
    synczoteronepomuk.cpp
    synckbibtexfile.cpp

    pipe/bibtexpipe.cpp
    pipe/bibtextoclipboardpipe.cpp
    pipe/bibtextonepomukpipe.cpp

    pipe/nepomukpipe.cpp
    pipe/nepomuktobibtexpipe.cpp
)

kde4_add_library( nbibio STATIC ${nbibio_LIB_SRCS} )
 
install( TARGETS nbibio  ${INSTALL_TARGETS_DEFAULT_ARGS} )

