add_subdirectory( stringreplacer )
add_subdirectory( xmltransformer )
add_subdirectory( sbd            )
add_subdirectory( talkerchooser  )

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


##### testfilter (test)

tde_add_check_executable( testfilter AUTOMOC

  SOURCES
        main.cpp
  LINK
     tdecore-shared
     tdeui-shared
     tdeio-shared
     kttsd-shared
  TEST
)


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_filterplugin.desktop
    DESTINATION ${SERVICETYPES_INSTALL_DIR}
    PO_DIR kttsd-desktops
)
