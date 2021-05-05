
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/kttsd/libkttsd
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${ALSA_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### libkttsd_alsaplugin (kpart)

tde_add_kpart( libkttsd_alsaplugin AUTOMOC

  SOURCES
        alsaplugin.cpp
        alsaplayer.cpp
  LINK
     kttsd-shared
     ${ALSA_LIBRARIES}

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_alsaplugin.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
    PO_DIR kttsd-desktops
)
