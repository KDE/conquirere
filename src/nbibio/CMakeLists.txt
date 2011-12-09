
set(nbibio_LIB_SRCS
    nbibexporter.cpp
    nbibexporterbibtex.cpp

    nbibimporter.cpp
    nbibimporterbibtex.cpp

    nbibsync.cpp
    synczoteronepomuk.cpp

    pipe/bibtexpipe.cpp
    pipe/bibtextoclipboardpipe.cpp
    pipe/bibtextonepomukpipe.cpp

    pipe/nepomukpipe.cpp
    pipe/nepomuktobibtexpipe.cpp
)

nepomuk_add_ontology_classes(nbibio_LIB_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig ${CMAKE_SOURCE_DIR}/nbib/sync.trig)
soprano_add_ontology(nbibio_LIB_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk::Vocabulary" "trig")
soprano_add_ontology(nbibio_LIB_SRCS ${CMAKE_SOURCE_DIR}/nbib/sync.trig "SYNC" "Nepomuk::Vocabulary" "trig")

kde4_add_library( nbibio STATIC ${nbibio_LIB_SRCS} )
 
install( TARGETS nbibio  ${INSTALL_TARGETS_DEFAULT_ARGS} )

