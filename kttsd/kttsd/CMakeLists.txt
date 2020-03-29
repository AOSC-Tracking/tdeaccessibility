
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

##### kttsd_kspeech

set( KSPEECH_KIDL kspeech.kidl )
set( KSPEECH_SKEL kspeech_skel )

add_custom_command(
    OUTPUT  ${KSPEECH_KIDL}
    COMMAND ${KDE3_DCOPIDL_EXECUTABLE} ${KTTS_KSPEECH_DIR}/kspeech.h > ${KSPEECH_KIDL}
    DEPENDS ${KTTS_KSPEECH_DIR}/kspeech.h
)
add_custom_target( kttsd-kspeech-kidl DEPENDS ${KSPEECH_KIDL} )

add_custom_command(
    OUTPUT  ${KSPEECH_SKEL}.cpp
    COMMAND ${KDE3_DCOPIDL2CPP_EXECUTABLE} --c++-suffix cpp --no-signals --no-stub ${KSPEECH_KIDL}
    DEPENDS kttsd-kspeech-kidl
)
add_custom_target( kttsd-kspeech-skel DEPENDS ${KSPEECH_SKEL}.cpp )


###### kttsd_kspeechsink

set( KSPEECHSINK_KIDL kspeechsink.kidl )
set( KSPEECHSINK_STUB kspeechsink_stub )

add_custom_command(
    OUTPUT  ${KSPEECHSINK_KIDL}
    COMMAND ${KDE3_DCOPIDL_EXECUTABLE} ${KTTS_KSPEECH_DIR}/kspeechsink.h > ${KSPEECHSINK_KIDL}
    DEPENDS ${KTTS_KSPEECH_DIR}/kspeechsink.h
)
add_custom_target( kttsd-kspeechsink-kidl DEPENDS ${KSPEECHSINK_KIDL} )

add_custom_command(
    OUTPUT  ${KSPEECHSINK_STUB}.h
    COMMAND ${KDE3_DCOPIDL2CPP_EXECUTABLE} --c++-suffix cpp --no-signals --no-skel ${KSPEECHSINK_KIDL}
    DEPENDS kttsd-kspeechsink-kidl
)
add_custom_target( kttsd-kspeechsink-stub DEPENDS ${KSPEECHSINK_STUB}.h )


##### kttsd

set( KTTSD_KIDL kttsd.kidl )
set( KTTSD_SKEL kttsd_skel )

add_custom_command(
    OUTPUT  ${KTTSD_KIDL}
    COMMAND ${KDE3_DCOPIDL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/kttsd/kttsd/kttsd.h > ${KTTSD_KIDL}
    DEPENDS ${CMAKE_SOURCE_DIR}/kttsd/kttsd/kttsd.h
)
add_custom_target( kttsd-kidl DEPENDS ${KTTSD_KIDL} )

add_custom_command(
    OUTPUT  ${KTTSD_SKEL}.cpp
    COMMAND ${KDE3_DCOPIDL2CPP_EXECUTABLE} --c++-suffix cpp --no-signals --no-stub ${KTTSD_KIDL}
    DEPENDS kttsd-kidl
)
add_custom_target( kttsd-skel DEPENDS ${KTTSD_SKEL}.cpp )


##### kttsd (executable)

tde_add_executable( kttsd AUTOMOC

  SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/${KSPEECH_SKEL}.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/${KTTSD_SKEL}.cpp
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
     
  DEPENDENCIES
     kttsd-kspeech-skel
     kttsd-kspeechsink-stub
     kttsd-skel

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
