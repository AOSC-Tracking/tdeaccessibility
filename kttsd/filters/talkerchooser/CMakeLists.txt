
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/kttsd/libkttsd
  ${CMAKE_BINARY_DIR}/kttsd/libkttsd
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### libkttsd_talkerchooserplugin (kpart)

tde_add_kpart( libkttsd_talkerchooserplugin AUTOMOC

  SOURCES
        talkerchooserconfwidget.ui
        talkerchooserconf.cpp
        talkerchooserproc.cpp
        talkerchooserplugin.cpp
  LINK
     kttsd-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_talkerchooserplugin.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)

install(
    FILES female_notifications_rc
    DESTINATION ${DATA_INSTALL_DIR}/kttsd/talkerchooser
)
