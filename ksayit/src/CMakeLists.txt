
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/ksayit/KTTSD_Lib
  ${KAYSAYIT_ARTS_INCLUDE}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)


##### ksayit (executable)

tde_add_executable( ksayit AUTOMOC

  SOURCES
        DocTreeView.ui
        fx_setup.ui
        KSayItView.ui
        doctreeviewimpl.cpp
        effectstack.cpp
        fxpluginhandler.cpp
        fxsetupimpl.cpp
        ksayit.skel
        ksayitsystemtray.cpp
        ksayitviewimpl.cpp
        main.cpp
        voicesetupdlg.cpp
        ksayit.cpp
        parasaxparser.cpp
        saxhandler.cpp
        ksayitbookmarkhandler.cpp
        contextmenuhandler.cpp
        docbookclasses.cpp
        docbookgenerator.cpp
        docbookparser.cpp
  LINK
     tdecore-shared
     tdeui-shared
     tdeio-shared
     KTTSD_Lib-shared
     ${KAYSAYIT_ARTS_LIBRARIES}

  DESTINATION ${BIN_INSTALL_DIR}
)

##### icons

tde_install_icons()


##### other data

install(
    FILES ksayitui.rc ContextMenus.xml
    DESTINATION ${DATA_INSTALL_DIR}/ksayit
)

tde_create_translated_desktop(
    SOURCE ksayit.desktop
    PO_DIR ksayit-desktops
)

install(
    FILES ksayit_fxplugin.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
)
