

add_subdirectory( core )
add_subdirectory( nbibio )
add_subdirectory( propertywidgets )

set(conquirere_SRCS
    main.cpp

    mainui/newprojectwizard.cpp
    mainui/loadproject.cpp
    mainui/welcomewidget.cpp
    mainui/librarywidget.cpp
    mainui/projecttreedelegate.cpp
    mainui/resourcetablewidget.cpp
    mainui/mainwindow.cpp
    mainui/bibtexexportdialog.cpp
    mainui/bibteximportwizard.cpp
    mainui/documentpreview.cpp
    mainui/websearchwidget.cpp
    mainui/qlibrarytreewidgetitem.cpp

    sidebar/sidebarwidget.cpp
    sidebar/sidebarcomponent.cpp
    sidebar/publicationwidget.cpp
    sidebar/referencewidget.cpp
    sidebar/listpublicationsdialog.cpp
    sidebar/documentwidget.cpp
    sidebar/notewidget.cpp
    sidebar/contactdialog.cpp
    sidebar/mergeresourceswidget.cpp
    sidebar/addchapterdialog.cpp
    sidebar/listpartswidget.cpp
    sidebar/serieswidget.cpp
)

kde4_add_ui_files( conquirere_SRCS

                   mainui/newprojectwizard.ui
                   mainui/loadproject.ui
                   mainui/welcomewidget.ui
                   mainui/librarywidget.ui
                   mainui/bibtexexportdialog.ui
                   mainui/bibteximportwizard.ui
                   mainui/documentpreview.ui
                   mainui/websearchwidget.ui

                   sidebar/referencewidget.ui
                   sidebar/listpublicationsdialog.ui
                   sidebar/publicationwidget.ui
                   sidebar/documentwidget.ui
                   sidebar/sidebarwidget.ui
                   sidebar/notewidget.ui
                   sidebar/contactdialog.ui
                   sidebar/mergeresourceswidget.ui
                   sidebar/addchapterdialog.ui
                   sidebar/listpartswidget.ui
                   sidebar/serieswidget.ui
)


nepomuk_add_ontology_classes(conquirere_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig)
soprano_add_ontology(conquirere_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk::Vocabulary" "trig")

kde4_add_app_icon(conquirere_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/hi*-app-conquirere.png")
kde4_add_executable(conquirere ${conquirere_SRCS})

target_link_libraries(conquirere
core
nbibio
propertywidgets
                      #${KDEPIMLIBS_AKONADI_LIBS}
                      ${KDE4_AKONADI_LIBS}
                      ${KDE4_KABC_LIBS}
                      #kabc
akonadi-contact
                      ${NEPOMUK_LIBRARIES}
                      nepomukquery
                      #nepomukcore
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KHTML_LIBS}
kbibtexio
kbibtexnetworking
kbibtexgui
kbibtexproc

nepomukdatamanagement-copy
)

install(TARGETS conquirere DESTINATION ${BIN_INSTALL_DIR})

install(FILES conquirere.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES conquirereui.rc    DESTINATION ${DATA_INSTALL_DIR}/conquirere)
