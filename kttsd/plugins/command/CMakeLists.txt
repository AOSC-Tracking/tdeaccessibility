
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


##### libkttsd_commandplugin (kpart)

tde_add_kpart( libkttsd_commandplugin AUTOMOC

  SOURCES
        commandconfwidget.ui
        commandconf.cpp
        commandproc.cpp
        commandplugin.cpp
  LINK
     kttsd-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_commandplugin.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
    PO_DIR kttsd-desktops
)
