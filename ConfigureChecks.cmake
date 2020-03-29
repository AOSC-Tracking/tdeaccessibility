###########################################
#                                         #
#  Improvements and feedback are welcome  #
#                                         #
#  This file is released under GPL >= 3   #
#                                         #
###########################################

# required stuff
find_package( TQt )
find_package( TDE )

tde_setup_architecture_flags( )

include(TestBigEndian)
test_big_endian(WORDS_BIGENDIAN)

tde_setup_largefiles( )


##### check for gcc visibility support

if( WITH_GCC_VISIBILITY )
  tde_setup_gcc_visibility( )
endif( WITH_GCC_VISIBILITY )


##### check for X11

find_package( X11 )


##### check for aRts

if( WITH_ARTS )
    pkg_search_module( ARTS arts )
    find_library( ARTSKDE artskde )

    if( ARTS_FOUND AND ARTSKDE )
        set( ARTS_LIBRARIES artskde-shared artsflow_idl soundserver_idl )
        set( ARTS_PLAYER_LIBRARIES "${ARTS_LIBRARIES}" kmedia2_idl )
     else()
        tde_message_fatal( "aRts support has been requested but was not found on your system." )
    endif()

    if( BUILD_KSAYIT )
        find_path( ARTS_INCLUDE_DIRS "arts/artsflow.h" )

        if( ARTS_INCLUDE_DIRS )
            set( KAYSAYIT_ARTS_INCLUDE "${ARTS_INCLUDE_DIRS}" )
            set( KAYSAYIT_ARTS_LIBRARIES "${ARTS_LIBRARIES}" artsflow )
         else()
            tde_message_fatal( "aRts headers are required for ksayit build but were not found on your system." )
        endif()
    endif()

    if( BUILD_KSAYIT_FREEVERB )
        find_path( ARTS_MODULES_DIR "arts/artsmodules.h" )

        if( ARTS_MODULES_DIR )
            set( ARTS_FREEVERB_PLUGIN_LIBRARIES "${ARTS_LIBRARIES}" artsflow artsmoduleseffects artsmodules )
         else()
            tde_message_fatal( freeverb support has been requested, but the <arts/artsmodules.h> header from tdemultimedia was not found )
        endif()
    endif()
endif( WITH_ARTS )


##### check for akode

if( WITH_AKODE )
    pkg_search_module( AKODE akode )

    if( NOT AKODE_FOUND )
        tde_message_fatal( "akode support has been requested but was not found on your system" )
    endif()
endif( WITH_AKODE )


##### check for alsa

if( WITH_ALSA )
    pkg_search_module( ALSA alsa )

    if( ALSA_FOUND )
        check_include_file(  "sys/time.h"        HAVE_SYS_TIME_H    )
        check_include_files( "sys/time.h;time.h" TIME_WITH_SYS_TIME )
     else()
       tde_message_fatal( "alsa support has been requested but was not found on your system." )
    endif()
endif( WITH_ALSA )


##### check for gstreamer

if( WITH_GSTREAMER )
    pkg_search_module( GSTREAMER gstreamer-1.0>=1.0.0 gstreamer-0.10>=0.10.0 )
    pkg_search_module( GLIB2 glib-2.0 )

    if( NOT GSTREAMER_FOUND )
        tde_message_fatal( "Gstreamer support has been requested but gstreamer was not found on your system." )
    endif()

    message( STATUS "gstreamer version: ${GSTREAMER_VERSION}" )
endif( WITH_GSTREAMER )


##### look for <kspeech.h> header

find_path( KTTS_KSPEECH_DIR NAMES kspeech.h
HINTS
      ${TQT_INCLUDE_DIRS}
      ${TDE_INCLUDE_DIR}
      ${TDE_INCLUDE_DIR}/tde
)
