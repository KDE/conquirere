
set(ontology_LIB_SRCS)

nepomuk_add_ontology_classes(ontology_LIB_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig ${CMAKE_SOURCE_DIR}/nbib/sync.trig)
soprano_add_ontology(ontology_LIB_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk2::Vocabulary" "trig")
soprano_add_ontology(ontology_LIB_SRCS ${CMAKE_SOURCE_DIR}/nbib/sync.trig "SYNC" "Nepomuk2::Vocabulary" "trig")

kde4_add_library( ontology STATIC ${ontology_LIB_SRCS} )

install( TARGETS ontology  ${INSTALL_TARGETS_DEFAULT_ARGS} )

