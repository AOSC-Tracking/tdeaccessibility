
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/ksayit/src
  ${ARTS_MODULES_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)

##### libFreeverb_plugin (module)

tde_add_library( libFreeverb_plugin MODULE AUTOMOC

  SOURCES
        freeverb_setup.ui
        freeverbsetupimpl.cpp
        ksayitfreeverblib.cpp
  LINK
     tdecore-shared
     tdeui-shared
     ${ARTS_FREEVERB_PLUGIN_LIBRARIES}

  DESTINATION ${PLUGIN_INSTALL_DIR}
)


##### other data

tde_create_translated_desktop(
    SOURCE ksayit_libFreeverb_service.desktop
    DESTINATION ${SERVICETYPES_INSTALL_DIR}
)

tde_create_translated_desktop(
    SOURCE ksayit_libFreeverb.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)
