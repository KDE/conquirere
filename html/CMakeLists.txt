########### install files ###############

FILE(GLOB html_files *.html)
FILE(GLOB css_files  *.css)

INSTALL(FILES ${html_files}
	DESTINATION ${DATA_INSTALL_DIR}/conquirere/html
)
INSTALL(FILES ${css_files}
	DESTINATION ${DATA_INSTALL_DIR}/conquirere/html
)
INSTALL(DIRECTORY images
	DESTINATION ${DATA_INSTALL_DIR}/conquirere/html
)

