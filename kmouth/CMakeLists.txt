add_subdirectory( icons          )
add_subdirectory( phrasebook     )
add_subdirectory( wordcompletion )
add_subdirectory( books          )

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/kmouth/phrasebook
  ${CMAKE_BINARY_DIR}/kmouth/wordcompletion
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### kmouth (executable)

tde_add_executable( kmouth AUTOMOC

  SOURCES
        preferencesui.ui
        texttospeechconfigurationui.ui
        texttospeechconfigurationwidget.cpp
        optionsdialog.cpp
        configwizard.cpp
        phraseedit.cpp
        phraselist.cpp
        speech.cpp
        texttospeechsystem.cpp
        phraselistitem.cpp
        kmouth.cpp
        main.cpp
  LINK
     tdecore-shared
     tdeui-shared
     tdeio-shared
     tdeprint-shared
     phrasebook-static
     wordcompletion-static

  DESTINATION ${BIN_INSTALL_DIR}
)


##### other data

install(
    FILES kmouthui.rc
    DESTINATION ${DATA_INSTALL_DIR}/kmouth
)

install(
    FILES kmouthrc
    DESTINATION ${CONFIG_INSTALL_DIR}
)

tde_create_translated_desktop(
    SOURCE kmouth.desktop
    PO_DIR kmouth-desktops
)
