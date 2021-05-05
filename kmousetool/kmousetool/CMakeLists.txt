add_subdirectory( pics )

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${X11_INCLUDE_DIR}
  ${X11_XTest_INCLUDE_PATH}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### kmousetool (executable)

tde_add_executable( kmousetool AUTOMOC

  SOURCES
        kmousetoolui.ui
        mtstroke.cpp
        kmousetool.cpp
        main.cpp
  LINK
     tdecore-shared
     tdeui-shared
     tdeio-shared
     ${X11_LIBRARIES}
     ${X11_XTest_LIB}
     ${X11_Xext_LIB}

  DESTINATION ${BIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kmousetool.desktop
    PO_DIR kmousetool-desktops
)

install(
    FILES mousetool_tap.wav
    DESTINATION ${DATA_INSTALL_DIR}/kmousetool/sounds
)
