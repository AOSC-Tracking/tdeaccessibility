
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/kttsd/libkttsd
  ${GSTREAMER_INCLUDE_DIRS}
  ${GLIB2_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### libkttsd_gstplugin (kpart)

tde_add_kpart( libkttsd_gstplugin AUTOMOC

  SOURCES
        gstplugin.cpp
        gstreamerplayer.cpp
  LINK
     kttsd-shared
     ${GSTREAMER_LIBRARIES}

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_gstplugin.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
