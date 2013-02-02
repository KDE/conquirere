
set(propertywidgets_LIB_SRCS
    kmultiitemedit.cpp

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
    eventedit.cpp
    collectionedit.cpp
    codeoflawedit.cpp
    courtreporteredit.cpp
    fileobjectedit.cpp
    fileobjecteditdialog.cpp
)

kde4_add_ui_files( propertywidgets_LIB_SRCS
                   fileobjectedit.ui
                   fileobjecteditdialog.ui
)

kde4_add_library(conquirere-propertywidgets STATIC ${propertywidgets_LIB_SRCS} )

