
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${TDE_INCLUDE_DIR}
  ${TQT_INCLUDE_DIRS}
)

link_directories(
  ${TQT_LIBRARY_DIRS}
  ${TDE_LIB_DIR}
)

add_compile_options( -fPIC )


##### ksayit_kspeech

set( KSPEECH_KIDL kspeech.kidl )
set( KSPEECH_STUB kspeech_stub )

add_custom_command(
    OUTPUT  ${KSPEECH_KIDL}
    COMMAND ${KDE3_DCOPIDL_EXECUTABLE} ${KTTS_KSPEECH_DIR}/kspeech.h > ${KSPEECH_KIDL}
    DEPENDS ${KTTS_KSPEECH_DIR}/kspeech.h
)
add_custom_target( ksayit-kspeech-kidl DEPENDS ${KSPEECH_KIDL} )

add_custom_command(
    OUTPUT  ${KSPEECH_STUB}.h ${KSPEECH_STUB}.cpp
    COMMAND ${KDE3_DCOPIDL2CPP_EXECUTABLE} --c++-suffix cpp --no-signals --no-skel ${KSPEECH_KIDL}
    DEPENDS ${KSPEECH_KIDL}
)
add_custom_target( ksayit-kspeech-stub DEPENDS ${KSPEECH_STUB}.cpp )


###### ksayit_kspeechsink

set( KSPEECHSINK_KIDL kspeechsink.kidl )
set( KSPEECHSINK_SKEL kspeechsink_skel )

add_custom_command(
    OUTPUT  ${KSPEECHSINK_KIDL}
    COMMAND ${KDE3_DCOPIDL_EXECUTABLE} ${KTTS_KSPEECH_DIR}/kspeechsink.h > ${KSPEECHSINK_KIDL}
    DEPENDS ${KTTS_KSPEECH_DIR}/kspeechsink.h
)
add_custom_target( ksayit-kspeechsink-kidl DEPENDS ${KSPEECHSINK_KIDL} )

add_custom_command(
    OUTPUT  ${KSPEECHSINK_SKEL}.cpp
    COMMAND ${KDE3_DCOPIDL2CPP_EXECUTABLE} --c++-suffix cpp --no-signals --no-stub ${KSPEECHSINK_KIDL}
    DEPENDS ${KSPEECHSINK_KIDL}
)
add_custom_target( ksayit-kspeechsink-skel DEPENDS ${KSPEECHSINK_SKEL}.cpp )


##### DUMMY_Lib (static)

tde_add_library( DUMMY_kspeech_lib STATIC

  SOURCES
        ${CMAKE_CURRENT_BINARY_DIR}/${KSPEECH_STUB}.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/${KSPEECHSINK_SKEL}.cpp
  LINK
     tdecore-shared

  DEPENDENCIES
     ksayit-kspeech-stub
     ksayit-kspeechsink-skel
)
