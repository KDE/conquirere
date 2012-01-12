

kde4_add_unit_test(bibtexnepomuktest
  bibtexnepomuktest.cpp
)

target_link_libraries(bibtexnepomuktest

  ${KDE4_KDECORE_LIBS}
  ${QT_QTTEST_LIBRARY}

)
