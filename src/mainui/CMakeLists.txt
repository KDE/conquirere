
set(mainui_LIB_SRCS
    librarymanager.cpp
    resourcetablewidget.cpp
    tableviewmenu.cpp
    mainwindow.cpp
    welcomewidget.cpp

    dialogs/newprojectwizard.cpp
    dialogs/loadproject.cpp
    dialogs/selectopenproject.cpp
    dialogs/dbcheckdialog.cpp

    docklets/librarywidget.cpp
    docklets/projecttreedelegate.cpp
    docklets/documentpreview.cpp
    docklets/qlibrarytreewidgetitem.cpp
    docklets/searchwidget.cpp

    settings/projectsettingsdialog.cpp
    settings/projectgeneralsettings.cpp
    settings/projectsyncsettings.cpp

    settings/conquireresettingsdialog.cpp
    settings/exportsettings.cpp
    settings/systemsyncsettings.cpp
    settings/providerselectionwidget.cpp
    settings/appearancesettings.cpp

    sync/bibtexexportdialog.cpp
    sync/bibteximportwizard.cpp
    sync/synczoterodialog.cpp
    sync/backgroundsync.cpp
    sync/storagesyncwizard.cpp
    sync/itemdeletedialog.cpp

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
    sidebar/libraryinfowidget.cpp
    sidebar/searchresultinfowidget.cpp
    sidebar/mailwidget.cpp
    sidebar/listannotationswidget.cpp
    sidebar/listcitedsources.cpp
)

kde4_add_ui_files( mainui_LIB_SRCS

                   dialogs/newprojectwizard.ui
                   dialogs/loadproject.ui
                   dialogs/selectopenproject.ui
                   dialogs/dbcheckdialog.ui

                   docklets/librarywidget.ui
                   docklets/documentpreview.ui
                   docklets/searchwidget.ui

                   settings/projectgeneralsettings.ui
                   settings/projectsyncsettings.ui
                   settings/exportsettings.ui
                   settings/systemsyncsettings.ui
                   settings/providerselectionwidget.ui
                   settings/appearancesettings.ui

                   sync/bibtexexportdialog.ui
                   sync/bibteximportwizard.ui
                   sync/storagesyncwizard.ui
                   sync/itemdeletedialog.ui

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
                   sidebar/libraryinfowidget.ui
                   sidebar/searchresultinfowidget.ui
                   sidebar/mailwidget.ui
                   sidebar/listannotationswidget.ui
                   sidebar/listcitedsources.ui
)

kde4_add_library( mainui STATIC ${mainui_LIB_SRCS} )
ADD_DEPENDENCIES(mainui nbibio)

target_link_libraries(mainui
# include static libs from conquirere
        ontology
        core
        nbibio
        propertywidgets
        onlinestorage
)
 
install( TARGETS mainui  ${INSTALL_TARGETS_DEFAULT_ARGS} )


