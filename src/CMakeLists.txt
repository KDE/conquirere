
set(conquirere_SRCS
    main.cpp

    core/library.cpp
    core/nepomukmodel.cpp
    core/resourcemodel.cpp
    core/publicationmodel.cpp
    core/publicationfiltermodel.cpp
    core/documentmodel.cpp
    core/notemodel.cpp
    core/bookmarkmodel.cpp
    core/ratingdelegate.cpp
    core/starrating.cpp

    mainui/newprojectdialog.cpp
    mainui/newprojectdialog.cpp
    mainui/welcomewidget.cpp
    mainui/librarywidget.cpp
    mainui/projecttreedelegate.cpp
    mainui/resourcetablewidget.cpp
    mainui/mainwindow.cpp
    mainui/bibtexexportdialog.cpp
    mainui/bibteximportdialog.cpp
    mainui/documentpreview.cpp

    propertywidgets/propertyedit.cpp
    propertywidgets/contactedit.cpp
    propertywidgets/stringedit.cpp
    propertywidgets/publicationedit.cpp
    propertywidgets/chapteredit.cpp
    propertywidgets/seriesedit.cpp
    propertywidgets/journaledit.cpp
    propertywidgets/volumenumberedit.cpp
    propertywidgets/fileobjectedit.cpp
    propertywidgets/organizationedit.cpp
    propertywidgets/tagedit.cpp
    propertywidgets/issnedit.cpp

    sidebar/sidebarwidget.cpp
    sidebar/sidebarcomponent.cpp
    sidebar/publicationwidget.cpp
    sidebar/referencewidget.cpp
    sidebar/listpublicationsdialog.cpp
    sidebar/documentwidget.cpp
    sidebar/notewidget.cpp

# later on create lib taget for this
    libnbibio/nbibexporter.cpp
    libnbibio/nbibexporterbibtex.cpp

    libnbibio/nbibimporter.cpp
    libnbibio/nbibimporterbibtex.cpp
    libnbibio/encoderlatex.cpp
    libnbibio/conflictmanager.cpp
)

kde4_add_ui_files( conquirere_SRCS

                   mainui/newprojectdialog.ui
                   mainui/welcomewidget.ui
                   mainui/librarywidget.ui
                   mainui/bibtexexportdialog.ui
                   mainui/bibteximportdialog.ui
                   mainui/documentpreview.ui

                   sidebar/referencewidget.ui
                   sidebar/listpublicationsdialog.ui
                   sidebar/publicationwidget.ui
                   sidebar/documentwidget.ui
                   sidebar/sidebarwidget.ui
                   sidebar/notewidget.ui
)


nepomuk_add_ontology_classes(conquirere_SRCS ONTOLOGIES ${CMAKE_SOURCE_DIR}/nbib/nbib.trig)
soprano_add_ontology(conquirere_SRCS ${CMAKE_SOURCE_DIR}/nbib/nbib.trig "NBIB" "Nepomuk::Vocabulary" "trig")

#kde4_add_app_icon(kalgebra_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/hi*-app-kalgebra.png")
kde4_add_executable(conquirere ${conquirere_SRCS})

target_link_libraries(conquirere
                      ${NEPOMUK_LIBRARIES}
                      nepomukquery
                      #nepomukcore
                      ${KDE4_KDECORE_LIBS}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_KPARTS_LIBS}
)

install(TARGETS conquirere DESTINATION ${BIN_INSTALL_DIR})

#install(FILES conquirere.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES conquirereui.rc    DESTINATION ${DATA_INSTALL_DIR}/conquirere)
