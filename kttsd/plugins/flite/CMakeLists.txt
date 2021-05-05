
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


##### libkttsd_fliteplugin (kpart)

tde_add_kpart( libkttsd_fliteplugin AUTOMOC

  SOURCES
        fliteconfwidget.ui
        fliteconf.cpp
        fliteproc.cpp
        fliteplugin.cpp
  LINK
     kttsd-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_fliteplugin.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
    PO_DIR kttsd-desktops
)
