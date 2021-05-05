
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/kttsd/libkttsd
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### libkttsd_sbdplugin (kpart)

tde_add_kpart( libkttsd_sbdplugin AUTOMOC

  SOURCES
        sbdconfwidget.ui
        sbdproc.cpp
        sbdconf.cpp
        sbdplugin.cpp
  LINK
     kttsd-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_sbdplugin.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
    PO_DIR kttsd-desktops
)

install(
    FILES standard_sbdrc polish_festival_sbdrc
    DESTINATION ${DATA_INSTALL_DIR}/kttsd/sbd
)
