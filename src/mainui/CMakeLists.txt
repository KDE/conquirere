
set(mainui_LIB_SRCS
    resourcetablewidget.cpp
    mainwindow.cpp
    welcomewidget.cpp

    dialogs/newprojectwizard.cpp
    dialogs/loadproject.cpp

    docklets/librarywidget.cpp
    docklets/projecttreedelegate.cpp
    docklets/documentpreview.cpp
    docklets/websearchwidget.cpp
    docklets/qlibrarytreewidgetitem.cpp

    sync/bibtexexportdialog.cpp
    sync/bibteximportwizard.cpp
    sync/synczoterodialog.cpp

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
    sidebar/eventwidget.cpp
)

kde4_add_ui_files( mainui_LIB_SRCS
                   welcomewidget.ui

                   dialogs/newprojectwizard.ui
                   dialogs/loadproject.ui

                   docklets/librarywidget.ui
                   docklets/documentpreview.ui
                   docklets/websearchwidget.ui

                   sync/bibtexexportdialog.ui
                   sync/bibteximportwizard.ui
                   sync/synczoterodialog.ui

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
                   sidebar/eventwidget.ui
)

kde4_add_library( mainui STATIC ${mainui_LIB_SRCS} )

target_link_libraries(mainui
# include static libs from conquirere
        ontology
        core
        nbibio
        propertywidgets
        onlinestorage
)
 
install( TARGETS mainui  ${INSTALL_TARGETS_DEFAULT_ARGS} )


