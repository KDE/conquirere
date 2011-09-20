
set(conquirere_SRCS
    main.cpp
    mainwindow.cpp

    core/project.cpp
    core/bibtexexporter.cpp

    ui/project/newprojectdialog.cpp
    ui/project/newprojectdialog.cpp
    ui/project/welcomewidget.cpp
    ui/project/projecttreewidget.cpp
    ui/project/projecttreedelegate.cpp
    ui/project/projectwidget.cpp
    ui/project/resourcemodel.cpp

    ui/semantic/labeledit.cpp
    ui/semantic/contactedit.cpp

    ui/sidebar/sidebarwidget.cpp
    ui/sidebar/sidebarcomponent.cpp
    ui/sidebar/publicationwidget.cpp
    ui/sidebar/referencewidget.cpp
)

kde4_add_ui_files( conquirere_SRCS

                   ui/project/newprojectdialog.ui
                   ui/project/welcomewidget.ui
                   ui/project/projecttreewidget.ui

                   ui/sidebar/referencewidget.ui
)


nepomuk_add_ontology_classes(conquirere_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig)
soprano_add_ontology(conquirere_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk::Vocabulary" "trig")

#kde4_add_app_icon(kalgebra_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/hi*-app-kalgebra.png")
kde4_add_executable(conquirere ${conquirere_SRCS})

target_link_libraries(conquirere
                      ${NEPOMUK_LIBRARIES}
                      nepomukquery
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
)

install(TARGETS conquirere DESTINATION ${BIN_INSTALL_DIR})

#install(FILES conquirere.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES conquirereui.rc    DESTINATION ${DATA_INSTALL_DIR}/conquirere)
