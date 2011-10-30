
set(propertywidgets_LIB_SRCS
    chapteredit.cpp
    contactedit.cpp
    dateedit.cpp
    fileobjectedit.cpp
    issnedit.cpp
    journaledit.cpp
    organizationedit.cpp
    proceedingsedit.cpp
    propertyedit.cpp
    publicationedit.cpp
    seriesedit.cpp
    stringedit.cpp
    tagedit.cpp
    volumenumberedit.cpp
)

nepomuk_add_ontology_classes(propertywidgets_LIB_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig)
soprano_add_ontology(propertywidgets_LIB_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk::Vocabulary" "trig")

kde4_add_library( propertywidgets STATIC ${propertywidgets_LIB_SRCS} )
 
install( TARGETS propertywidgets  ${INSTALL_TARGETS_DEFAULT_ARGS} )

