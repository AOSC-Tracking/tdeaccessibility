
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


##### tdetexteditor_kttsd (kpart)

tde_add_kpart( tdetexteditor_kttsd AUTOMOC

  SOURCES
        katekttsd.cpp
  LINK
     tdecore-shared
     tdeui-shared
     tdetexteditor-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

install(
    FILES tdetexteditor_kttsdui.rc
    DESTINATION ${DATA_INSTALL_DIR}/tdetexteditor_kttsd
)

tde_create_translated_desktop(
    SOURCE tdetexteditor_kttsd.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
