
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


##### libkttsjobmgrpart (kpart)

tde_add_kpart( libkttsjobmgrpart MODULE AUTOMOC

  SOURCES
        kttsjobmgr.cpp
  LINK
     tdeio-shared
     tdeparts-shared
     kttsd-shared
     DUMMY_kspeech_lib-static

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsjobmgr.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
