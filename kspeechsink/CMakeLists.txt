
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


##### kspeech_stub (static)

tde_add_library( kspeech_stub STATIC_PIC

  SOURCES
        ${KTTS_KSPEECH_DIR}/kspeech.stub
        ${KTTS_KSPEECH_DIR}/kspeechsink.skel

  EXCLUDE_FROM_ALL
)


##### kspeech_skel (static)

tde_add_library( kspeech_skel STATIC_PIC

  SOURCES
        ${KTTS_KSPEECH_DIR}/kspeech.skel
        ${KTTS_KSPEECH_DIR}/kspeechsink.stub

  EXCLUDE_FROM_ALL
)
