

add_subdirectory( core )
add_subdirectory( nbibio )
add_subdirectory( propertywidgets )
add_subdirectory( onlinestorage )

set(conquirere_SRCS
    main.cpp

    mainui/resourcetablewidget.cpp
    mainui/mainwindow.cpp
    mainui/welcomewidget.cpp

    mainui/dialogs/newprojectwizard.cpp
    mainui/dialogs/loadproject.cpp

    mainui/docklets/librarywidget.cpp
    mainui/docklets/projecttreedelegate.cpp
    mainui/docklets/documentpreview.cpp
    mainui/docklets/websearchwidget.cpp
    mainui/docklets/qlibrarytreewidgetitem.cpp

    mainui/sync/bibtexexportdialog.cpp
    mainui/sync/bibteximportwizard.cpp

    mainui/sync/synczoterodialog.cpp

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

                   mainui/welcomewidget.ui

                   mainui/dialogs/newprojectwizard.ui
                   mainui/dialogs/loadproject.ui

                   mainui/docklets/librarywidget.ui
                   mainui/docklets/documentpreview.ui
                   mainui/docklets/websearchwidget.ui

                   mainui/sync/bibtexexportdialog.ui
                   mainui/sync/bibteximportwizard.ui
                   mainui/sync/synczoterodialog.ui

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

nepomuk_add_ontology_classes(conquirere_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig ${CMAKE_SOURCE_DIR}/nbib/sync.trig)
soprano_add_ontology(conquirere_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk::Vocabulary" "trig")
soprano_add_ontology(conquirere_SRCS ${CMAKE_SOURCE_DIR}/nbib/sync.trig "SYNC" "Nepomuk::Vocabulary" "trig")

kde4_add_app_icon(conquirere_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/hi*-app-conquirere.png")
kde4_add_executable(conquirere ${conquirere_SRCS})

target_link_libraries(conquirere
core
nbibio
propertywidgets
onlinestorage
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
