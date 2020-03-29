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


##### phrasebook (static)

tde_add_library( phrasebook STATIC_PIC AUTOMOC

  SOURCES
        buttonboxui.ui
        phrasetree.cpp
        phrasebookparser.cpp
        phrasebookdialog.cpp
        phrasebook.cpp
)


install(
    FILES phrasebookdialogui.rc
    DESTINATION ${DATA_INSTALL_DIR}/kmouth
)
