include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### wordcompletion (static)

tde_add_library( wordcompletion STATIC_PIC AUTOMOC

  SOURCES
        wordcompletionui.ui
        creationsourceui.ui
        creationsourcedetailsui.ui
        kdedocsourceui.ui
        wordlist.cpp
        wordcompletion.cpp
        wordcompletionwidget.cpp
        dictionarycreationwizard.cpp
        klanguagebutton.cpp
        klanguagebuttonhelper.cpp
)
