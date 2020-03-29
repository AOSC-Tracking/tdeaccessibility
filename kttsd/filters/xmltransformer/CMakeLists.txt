
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


##### libkttsd_xmltransformerplugin (kpart)

tde_add_kpart( libkttsd_xmltransformerplugin AUTOMOC

  SOURCES
        xmltransformerconfwidget.ui
        xmltransformerconf.cpp
        xmltransformerproc.cpp
        xmltransformerplugin.cpp
  LINK
     kttsd-shared

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_xmltransformerplugin.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)

install(
    FILES xhtml2ssml.xsl xhtml2ssml_simple.xsl
    DESTINATION ${DATA_INSTALL_DIR}/kttsd/xmltransformer
)
