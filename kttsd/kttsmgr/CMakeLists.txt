
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/kspeechsink
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### kttsmgr (executable)

tde_add_executable( kttsmgr AUTOMOC

  SOURCES
        kttsmgr.cpp
  LINK
     tdecore-shared
     tdeui-shared
     tdeio-shared
     tdeutils-shared
     kspeech_stub-static

  DESTINATION ${BIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop( kttsmgr.desktop )
