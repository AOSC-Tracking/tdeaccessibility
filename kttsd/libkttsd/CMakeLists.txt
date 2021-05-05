
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

##### kttsd (shared)

tde_add_library( kttsd SHARED AUTOMOC

  SOURCES
        selecttalkerwidget.ui
        pluginproc.cpp
        pluginconf.cpp
        testplayer.cpp
        stretcher.cpp
        talkercode.cpp
        filterproc.cpp
        filterconf.cpp
        utils.cpp
        selecttalkerdlg.cpp
        notify.cpp
  LINK
     tdecore-shared
     tdeio-shared
     tdeui-shared

  VERSION 1.0.0
     
  DESTINATION ${LIB_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_synthplugin.desktop
    DESTINATION ${SERVICETYPES_INSTALL_DIR}
    PO_DIR kttsd-desktops
)
