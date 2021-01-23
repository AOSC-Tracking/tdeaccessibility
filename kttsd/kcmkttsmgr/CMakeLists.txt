
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/kttsd/libkttsd
  ${CMAKE_BINARY_DIR}/kttsd/libkttsd
  ${CMAKE_BINARY_DIR}/kspeechsink
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### kcm_kttsd (kpart)

tde_add_kpart( kcm_kttsd MODULE AUTOMOC

  SOURCES
        kcmkttsmgrwidget.ui
        addtalkerwidget.ui
        selecteventwidget.ui
        kcmkttsmgr.cpp
        addtalker.cpp
        selectevent.cpp
  LINK
     tdeio-shared
     kttsd-shared
     kspeech_stub-static

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kcmkttsd_testmessage.desktop
    DESTINATION ${DATA_INSTALL_DIR}/kttsd
)

tde_create_translated_desktop( kcmkttsd.desktop )
