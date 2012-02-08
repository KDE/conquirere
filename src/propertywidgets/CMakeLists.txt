
set(propertywidgets_LIB_SRCS
    chapteredit.cpp
    contactedit.cpp
    dateedit.cpp
    issnedit.cpp
    organizationedit.cpp
    propertyedit.cpp
    publicationedit.cpp
    seriesedit.cpp
    stringedit.cpp
    tagedit.cpp
    volumenumberedit.cpp
    adddataobject.cpp
    eventedit.cpp
    collectionedit.cpp
    codeoflawedit.cpp
    courtreporteredit.cpp
    fileobjectedit.cpp
    fileobjecteditdialog.cpp
)

kde4_add_ui_files( propertywidgets_LIB_SRCS
                   adddataobject.ui
                   fileobjectedit.ui
                   fileobjecteditdialog.ui
)

kde4_add_library( propertywidgets STATIC ${propertywidgets_LIB_SRCS} )
 
install( TARGETS propertywidgets  ${INSTALL_TARGETS_DEFAULT_ARGS} )

