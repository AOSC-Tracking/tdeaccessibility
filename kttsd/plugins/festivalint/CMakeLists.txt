
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


##### libkttsd_festivalintplugin (kpart)

tde_add_kpart( libkttsd_festivalintplugin AUTOMOC

  SOURCES
        festivalintconfwidget.ui
        festivalintconf.cpp
        festivalintproc.cpp
        festivalintplugin.cpp
  LINK
     kttsd-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_festivalintplugin.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
    PO_DIR kttsd-desktops
)

install(
    FILES SSMLtoSable.xsl
    DESTINATION ${DATA_INSTALL_DIR}/kttsd/festivalint/xslt
)

install(
    FILES voices sabletowave.scm
    DESTINATION ${DATA_INSTALL_DIR}/kttsd/festivalint
)
