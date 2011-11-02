
set(nbibio_LIB_SRCS
    nbibexporter.cpp
    nbibexporterbibtex.cpp

    nbibimporter.cpp
    nbibimporterbibtex.cpp
    conflictmanager.cpp

    pipe/bibtextoclipboardpipe.cpp
    pipe/bibtextonepomukpipe.cpp
    pipe/nepomuktobibtexpipe.cpp
    pipe/bibtexpipe.cpp
)

nepomuk_add_ontology_classes(nbibio_LIB_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig)
soprano_add_ontology(nbibio_LIB_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk::Vocabulary" "trig")

kde4_add_library( nbibio STATIC ${nbibio_LIB_SRCS} )
 
install( TARGETS nbibio  ${INSTALL_TARGETS_DEFAULT_ARGS} )

