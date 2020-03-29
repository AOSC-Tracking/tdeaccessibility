
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/kttsd/libkttsd
  ${ARTS_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### libkttsd_artsplugin (kpart)

tde_add_kpart( libkttsd_artsplugin AUTOMOC

  SOURCES
        artsplugin.cpp
        artsplayer.cpp
  LINK
     kttsd-shared
     ${ARTS_PLAYER_LIBRARIES}

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_artsplugin.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
