
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


##### kttsd (executable)

tde_add_executable( kttsd AUTOMOC

  SOURCES
        kttsd.skel
        main.cpp
        kttsd.cpp
        speaker.cpp
        speechdata.cpp
        threadedplugin.cpp
        ssmlconvert.cpp
        filtermgr.cpp
        talkermgr.cpp
  LINK
     tdecore-shared
     tdeui-shared
     tdeio-shared
     kttsd-shared
     kspeech_skel-static

  DESTINATION ${BIN_INSTALL_DIR}
)


##### other data

install(
    FILES SSMLtoPlainText.xsl
    DESTINATION ${DATA_INSTALL_DIR}/kttsd/xslt
)

tde_create_translated_desktop(
    SOURCE kttsd.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
