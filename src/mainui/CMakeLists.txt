
set(mainui_LIB_SRCS
    resourcetablewidget.cpp
    tableviewmenu.cpp
    mainwindow.cpp
    welcomewidget.cpp
    splashscreen.cpp

    dialogs/newprojectwizard.cpp
    dialogs/loadproject.cpp
    dialogs/selectopenproject.cpp
    dialogs/dbcheckdialog.cpp

    docklets/librarywidget.cpp
    docklets/projecttreedelegate.cpp
    docklets/documentpreview.cpp
    docklets/documentpreviewtab.cpp
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
    settings/librarysettings.cpp

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
                   docklets/documentpreviewtab.ui
                   docklets/searchwidget.ui

                   settings/projectgeneralsettings.ui
                   settings/projectsyncsettings.ui
                   settings/exportsettings.ui
                   settings/systemsyncsettings.ui
                   settings/providerselectionwidget.ui
                   settings/appearancesettings.ui
                   settings/librarysettings.ui

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

kde4_add_library(conquirere-mainui STATIC ${mainui_LIB_SRCS} )
ADD_DEPENDENCIES(conquirere-mainui conquirere-config)
ADD_DEPENDENCIES(conquirere-mainui conquirere-core)
ADD_DEPENDENCIES(conquirere-mainui conquirere-nbibio)

target_link_libraries(conquirere-mainui

# include static libs from conquirere
        conquirere-ontology
        conquirere-config
        conquirere-core
        conquirere-nbibio
        conquirere-propertywidgets

        ${NEPOMUK_WEBMINER_LIBS}
        ${NEPOMUK_CORE_LIBRARY}
)
 
#install( TARGETS mainui  ${INSTALL_TARGETS_DEFAULT_ARGS} )


