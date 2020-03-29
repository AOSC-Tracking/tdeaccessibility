add_subdirectory( images )

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${X11_INCLUDE_DIR}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### kmag (executable)

tde_add_executable( kmag AUTOMOC

  SOURCES
        kmagzoomview.cpp
        kmagselrect.cpp
        kmag.cpp
        main.cpp
  LINK
     tdecore-shared
     tdeui-shared
     tdeio-shared
     tdeprint-shared

  DESTINATION ${BIN_INSTALL_DIR}
)


##### icons

tde_install_icons( kmag )


##### other data

install(
    FILES kmagui.rc
    DESTINATION ${DATA_INSTALL_DIR}/kmag
)

tde_create_translated_desktop( kmag.desktop )
