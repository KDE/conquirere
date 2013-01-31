

set(ontology_LIB_SRCS)

nepomuk_add_ontology_classes(ontology_LIB_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/ontology/nbib.trig ${CMAKE_SOURCE_DIR}/ontology/sync.trig)
soprano_add_ontology(ontology_LIB_SRCS ${CMAKE_SOURCE_DIR}/ontology/nbib.trig "NBIB" "Nepomuk2::Vocabulary" "trig")
soprano_add_ontology(ontology_LIB_SRCS ${CMAKE_SOURCE_DIR}/ontology/sync.trig "SYNC" "Nepomuk2::Vocabulary" "trig")

kde4_add_library( ontology STATIC ${ontology_LIB_SRCS} )

install( TARGETS ontology  ${INSTALL_TARGETS_DEFAULT_ARGS} )

# -------------------------------------------
# install the ontology
# -------------------------------------------

macro(INSTALL_ONTOLOGY _name _group)
  set(ONTO_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/ontology/${_group})
  configure_file("${_name}.ontology.in" "${CMAKE_CURRENT_BINARY_DIR}/${_name}.ontology")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${_name}.ontology" DESTINATION ${ONTO_INSTALL_DIR})
  install(FILES "${_name}.trig" DESTINATION ${ONTO_INSTALL_DIR})
endmacro(INSTALL_ONTOLOGY)

install_ontology(sync kde)
install_ontology(nbib nepomuk)