
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


##### KTTSD_Lib (shared)

tde_add_library( KTTSD_Lib SHARED AUTOMOC

  SOURCES
        KTTSDlibSetup.ui
        kttsdlib.cpp
        kttsdlibsetupimpl.cpp
        kttsdlibtalker2.cpp
  LINK
     tdeui-shared
     tdecore-shared
     DUMMY_kspeech_lib-static

  VERSION 0.0.1

  DESTINATION ${LIB_INSTALL_DIR}
)
