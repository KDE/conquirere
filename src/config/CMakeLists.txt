

kde4_add_kcfg_files(config_LIB_SRCS conquirere.kcfgc)
install(FILES conquirere.kcfg DESTINATION ${KCFG_INSTALL_DIR})

kde4_add_library( conquirere-config STATIC ${config_LIB_SRCS} )

