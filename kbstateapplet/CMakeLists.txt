add_subdirectory( images )

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${X11_INCLUDE_DIR}
  ${X11_Xkb_INCLUDE_PATH}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### kbstate_panelapplet (kpart)

tde_add_kpart( kbstate_panelapplet MODULE AUTOMOC

  SOURCES
        kbstate.cpp
  LINK
     tdeui-shared
     tdecore-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kbstateapplet.desktop
    DESTINATION ${DATA_INSTALL_DIR}/kicker/applets
    PO_DIR kbstateapplet-desktops
)
